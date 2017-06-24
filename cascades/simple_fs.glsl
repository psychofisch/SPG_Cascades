#version 440 core
out vec4 fragColor;

#define BLOCKER_SEARCH_NUM_SAMPLES 16
#define PCF_NUM_SAMPLES 16
#define NEAR_PLANE 0.0
#define LIGHT_WORLD_SIZE .5
#define LIGHT_FRUSTUM_WIDTH 300 //3.75

#define LIGHT_SIZE_UV 0.005//(LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

const float normalFactor = 1.0;

const vec2 poissonDisk[16] = vec2[](
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 ),
	vec2( -0.91588581, 0.45771432 ),
	vec2( -0.81544232, -0.87912464 ),
	vec2( -0.38277543, 0.27676845 ),
	vec2( 0.97484398, 0.75648379 ),
	vec2( 0.44323325, -0.97511554 ),
	vec2( 0.53742981, -0.47373420 ),
	vec2( -0.26496911, -0.41893023 ),
	vec2( 0.79197514, 0.19090188 ),
	vec2( -0.24188840, 0.99706507 ),
	vec2( -0.81409955, 0.91437590 ),
	vec2( 0.19984126, 0.78641367 ),
	vec2( 0.14383161, -0.14100790 )
);

// in Data{
	// vec3 position;
	// vec3 normal;
	// vec4 color;
	// mat4 pv;
// } dataIn;

in vData{
	vec3 position;
	vec3 normal;
	vec4 fragPosLightSpace;
} dataIn;

in mat3 TBN;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform sampler2D diffuseTexture;
uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D displaceTexture;
uniform int shadowMode;
uniform int displaceMode;

vec2 parallaxMapping(vec2 uvCoords, vec3 viewDir, float scale, vec3 normal, int mode)
{
	//return uvCoords;
	int initSteps = 8;
	int refineSteps = 4;
	
	float layerDepth = 1.0 / initSteps;
	float depthCurrent = 0.0;
	float normalSign = sign(normal[mode]);
	vec2 shift;
	
	if(mode == 0)
		shift = vec2(viewDir.x, -viewDir.y);
	else if(/* (mode == 0 && normalSign < 0) ||  */
	(mode == 1 && normalSign < 0) || (mode == 2 && normalSign > 0))
		shift = vec2(viewDir.x, -viewDir.y);
	else if((mode == 1 && normalSign > 0) || (mode == 2 && normalSign < 0))
		shift = -vec2(viewDir.x, viewDir.y);
	
	if(mode == 1)
	 shift *= -1;
		
	shift *= normalFactor * normalSign * 0.5;
	vec2 uvCoordsDiff = shift / initSteps;
	
	vec2 uvCoordsCurrent = uvCoords;
	float depthMapValue = texture(displaceTexture, uvCoordsCurrent * scale).r;
	
	//init search
	for(int i = 0; i < initSteps; ++i)
	{
		if(depthMapValue <= depthCurrent)
			break;
	
		uvCoordsCurrent -= uvCoordsDiff;
		depthMapValue = texture(displaceTexture, uvCoordsCurrent * scale).r;
		depthCurrent += layerDepth;
	}
	
	//refinement
	depthCurrent -= layerDepth;
	uvCoordsCurrent += uvCoordsDiff;
	depthMapValue = 1.0; //no need to sample again, because it IS higher than depthCurrent
	layerDepth /= refineSteps;
	uvCoordsDiff /= refineSteps;
	for(int i = 0; i < refineSteps; ++i)
	{
		if(depthMapValue < depthCurrent)
			break;
	
		uvCoordsCurrent -= uvCoordsDiff;
		depthMapValue = texture(displaceTexture, uvCoordsCurrent * scale).r;
		depthCurrent += layerDepth;
	}
	
	return uvCoordsCurrent;
}

float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation
{
	return (zReceiver - zBlocker) / zBlocker;
}

void FindBlocker(out float avgBlockerDepth, out float numBlockers, vec2 uv, float zReceiver )
{
	//This uses similar triangles to compute what
	//area of the shadow map we should search
	float searchWidth = LIGHT_SIZE_UV /* * (zReceiver/* - NEAR_PLANE) / zReceiver*/;
	float blockerSum = 0;
	numBlockers = 0;
	for( int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i )
	{
		//float shadowMapDepth = tDepthMap.SampleLevel(PointSampler, uv + poissonDisk[i] * searchWidth, 0); //DX
		float shadowMapDepth = texture(depthMap, uv + poissonDisk[i] * searchWidth).r;
		if ( shadowMapDepth < zReceiver )
		{
			blockerSum += shadowMapDepth;
			numBlockers++;
		}
	}
	avgBlockerDepth = blockerSum / numBlockers;
}

float PCF_Filter(vec2 uv, float zReceiver, float filterRadiusUV )
{
	float sum = 0.0f;
	for ( int i = 0; i < PCF_NUM_SAMPLES; ++i )
	{
		vec2 offset = poissonDisk[i] * filterRadiusUV;
		//sum += tDepthMap.SampleCmpLevelZero(PCF_Sampler, uv + offset, zReceiver);
		sum += zReceiver - 0.005 > texture(depthMap, uv + offset).r ? 1.0 : 0.0;
	}
	return sum / PCF_NUM_SAMPLES;
}

float PCSS (vec3 coords)
{
	
	vec2 uv = coords.xy;
	float zReceiver = coords.z; // Assumed to be eye-space z in this code
	
	/* STEP 1: blocker search */
	float avgBlockerDepth = 0;
	float numBlockers = 0;
	FindBlocker( avgBlockerDepth, numBlockers, uv, zReceiver );
	
	if( numBlockers < 1 )
	/* There are no occluders so early out (this saves filtering)*/
		return 0.0;
	
	/* STEP 2: penumbra size */
	float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
	float filterRadiusUV = penumbraRatio * LIGHT_SIZE_UV /** NEAR_PLANE*/ / coords.z;
	
	/* STEP 3: filtering */
	return PCF_Filter( uv, zReceiver, filterRadiusUV );
}

float shadowCalc(vec4 fragPosLightSpace, vec3 norm)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	if(projCoords.z > 1.0)
		return 0.0;

	//int shadowMode = 0;
	if(shadowMode == 0)
		return PCSS(projCoords);
	else if(shadowMode == 1)
		return PCF_Filter( projCoords.xy, projCoords.z, 0.002 );
	else		
		return (projCoords.z - 0.005 > texture(depthMap, projCoords.xy).r) ? 1.0 : 0.0;
}

vec2 parallaxMapping(vec2 uvCoords, vec3 viewDir, float scale, float normalSign, int mode)
{
	//return uvCoords;
	ivec2 displacementSteps = ivec2(8, 4);
	int initSteps = displacementSteps.x;
	int refineSteps = displacementSteps.y;
	
	float layerDepth = 1.0 / initSteps;
	float depthCurrent = 0.0;
	vec2 shift;
	
	if(mode == 0)
		shift = vec2(viewDir.x, -viewDir.y);
	else if(/* (mode == 0 && normalSign < 0) ||  */
	(mode == 1 && normalSign < 0) || (mode == 2 && normalSign > 0))
		shift = vec2(viewDir.x, -viewDir.y);
	else if((mode == 1 && normalSign > 0) || (mode == 2 && normalSign < 0))
		shift = -vec2(viewDir.x, viewDir.y);
	
	if(mode == 1)
	 shift *= -1;
		
	shift *= normalFactor * normalSign * 0.5;
	
	vec2 uvCoordsDiff = shift / initSteps;
	
	vec2 uvCoordsCurrent = uvCoords;
	float depthMapValue = texture(displaceTexture, uvCoordsCurrent * scale).r;
	
	//init search
	for(int i = 0; i < initSteps; ++i)
	{
		if(depthMapValue <= depthCurrent)
			break;
	
		uvCoordsCurrent -= uvCoordsDiff;
		depthMapValue = texture(displaceTexture, uvCoordsCurrent * scale).r;
		depthCurrent += layerDepth;
	}
	
	//refinement
	depthCurrent -= layerDepth;
	uvCoordsCurrent += uvCoordsDiff;
	depthMapValue = 1.0; //no need to sample again, because it IS higher than depthCurrent
	layerDepth /= refineSteps;
	uvCoordsDiff /= refineSteps;
	for(int i = 0; i < refineSteps; ++i)
	{
		if(depthMapValue < depthCurrent)
			break;
	
		uvCoordsCurrent -= uvCoordsDiff;
		depthMapValue = texture(displaceTexture, uvCoordsCurrent * scale).r;
		depthCurrent += layerDepth;
	}
	
	return uvCoordsCurrent;
}

void main()
{
	vec3 lightColor = vec3(1.0);
	vec3 normal = dataIn.normal;
	
	vec3 lightDir = normalize(lightPos - dataIn.position);
	vec3 viewDir = normalize(cameraPos - dataIn.position);
	
	//tri-planar
	vec3 blend = abs(dataIn.normal);
	blend = normalize(max(blend, 0.00001));
	float b = (blend.x + blend.y + blend.z);
	blend /= vec3(b, b, b);
	
	vec3 xColorCode = vec3(1.0, 0, 0);
	vec3 yColorCode = vec3(0, 1.0, 0);
	vec3 zColorCode = vec3(0, 0, 1.0);
	
	float scale = 0.05;
	float colorFactor = 1.0;
	
	vec2 xCoords = dataIn.position.zy;
	vec2 yCoords = dataIn.position.zx;
	vec2 zCoords = dataIn.position.xy;
	
	if(displaceMode == 1)
	{
		xCoords = parallaxMapping(dataIn.position.zy, TBN * viewDir, scale, sign(normal.x), 0);
		yCoords = parallaxMapping(dataIn.position.xz, TBN * viewDir, scale, sign(normal.y), 1);
		zCoords = parallaxMapping(dataIn.position.xy, TBN * viewDir, scale, sign(normal.z), 2);
	}
	
	vec3 xColor = texture(diffuseTexture, xCoords * scale).rgb * max(xColorCode, colorFactor);
	vec3 yColor = texture(diffuseTexture, yCoords * scale).rgb * max(yColorCode, colorFactor);
	vec3 zColor = texture(diffuseTexture, zCoords * scale).rgb * max(zColorCode, colorFactor);
	
	vec3 color = xColor * blend.x * 1 + yColor * blend.y * 1 + zColor * blend.z * 1;
	//vec3 normal = xNormal * blend.x * 1 + yNormal * blend.y * 1 + zNormal * blend.z * 1;
	
	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor * color;
	
	// Diffuse 
	float diff = max(dot(lightDir, normal), 0.0);
	//vec4 diffuse = dataIn.color * diff;
	vec3 diffuse = diff * lightColor * color;

	// Specular
	float specularStrength = 0.3;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = specularStrength * spec * vec3(1.0)/** lightColor*/;  
	
	float shadow = shadowCalc(dataIn.fragPosLightSpace, normal);
	//shadow = 0.0;
	
	vec3 lighting = (diffuse + specular) * (1.0 - shadow) + ambient;
	fragColor = vec4(lighting, 1.0);
	//fragColor = vec4(abs(normalize(dataIn.normal)), 1.0);
	//fragColor = dataIn.color;
	//fragColor = vec4(1.f, 0.0f, 0.0f, 1.0f);
}
