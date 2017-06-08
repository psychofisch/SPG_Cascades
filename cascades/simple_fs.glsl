#version 440 core
out vec4 fragColor;

// in Data{
	// vec3 position;
	// vec3 normal;
	// vec4 color;
	// mat4 pv;
// } dataIn;

in vData{
	mat4 p;
	mat4 v;
	vec3 position;
	vec3 normal;
} dataIn;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform sampler2D diffuseTexture;
uniform mat4 lightMatrix;
uniform sampler2D depthMap;

float shadowCalc(vec3 norm)
{
	vec4 fragPosLightSpace = lightMatrix * vec4(dataIn.position, 1.0);
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	if(projCoords.z > 1.0)
		return 0.0;
	
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
	float bias = max(0.005 * (1.0 - dot(norm, normalize(lightPos - dataIn.position))), 0.005);  

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	
	shadow /= 9.0;	
	
    return shadow;
}

void main()
{
	vec3 lightColor = vec3(1.0);
	vec3 normal = dataIn.normal;

	//tri-planar
	vec3 blend = abs(dataIn.normal);
	blend = normalize(max(blend, 0.00001));
	float b = (blend.x + blend.y + blend.z);
	blend /= vec3(b, b, b);
	
	vec3 xColorCode = vec3(1.0, 0, 0);
	vec3 yColorCode = vec3(0, 1.0, 0);
	vec3 zColorCode = vec3(0, 0, 1.0);
	
	float scale = 0.1;
	float colorFactor = 1.0;
	
	vec2 xCoords = dataIn.position.zy;
	vec3 xColor = texture(diffuseTexture, xCoords * scale).rgb * max(xColorCode, colorFactor);
	
	vec2 yCoords = dataIn.position.zx;
	vec3 yColor = texture(diffuseTexture, yCoords * scale).rgb * max(yColorCode, colorFactor);
	
	vec2 zCoords = dataIn.position.xy;
	vec3 zColor = texture(diffuseTexture, zCoords * scale).rgb * max(zColorCode, colorFactor);
	
	vec3 color = xColor * blend.x * 1 + yColor * blend.y * 1 + zColor * blend.z * 1;
	
	// Ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	
	// Diffuse 
	vec3 lightDir = normalize(lightPos - dataIn.position);
	vec3 viewDir = normalize(cameraPos - dataIn.position);
	float diff = max(dot(lightDir, normal), 0.0);
	//vec4 diffuse = dataIn.color * diff;
	vec3 diffuse = diff * lightColor * color;

	// Specular
	float specularStrength = 0.3;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = specularStrength * spec * vec3(1.0)/** lightColor*/;  
	
	float shadow = shadowCalc(normal);
	//shadow = 0.0;
	
	vec3 lighting = (diffuse + specular) * (1.0 - shadow) + ambient;
	fragColor = vec4(lighting, 1.0);
	//fragColor = vec4(abs(normalize(dataIn.normal)), 1.0);
	//fragColor = dataIn.color;
	//fragColor = vec4(1.f, 0.0f, 0.0f, 1.0f);
}
