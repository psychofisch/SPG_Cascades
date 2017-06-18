#version 440 core
layout(triangles/*, point_mode*/, ccw) in;

//in vec3 tcPosition[];
in tcData{
	vec3 position;
	vec3 normal;
	float tessLevel;
} tcDataIn[];

out vData{
	mat4 p;
	mat4 v;
	vec3 position;
	vec3 normal;
	vec4 fragPosLightSpace;
} dataOut;

// out vec3 tePosition;
// out vec3 teNormal;
// out vec3 tePatchDistance;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightMatrix;
uniform sampler2D displaceTexture;

void main()
{
	dataOut.p = projection;
	dataOut.v = view;
	
	vec3 tessPos = vec3(0.0);
	vec3 tessNorm = vec3(0.0);
	float offset = 0.0f;
	offset = max(0.0, 0.4 - length(gl_TessCoord - vec3(0.3333)));
	tessPos += gl_TessCoord.x * tcDataIn[0].position;
    tessPos += gl_TessCoord.y * tcDataIn[1].position;
    tessPos += gl_TessCoord.z * tcDataIn[2].position;
    //tePatchDistance = gl_TessCoord;
	
	tessNorm += gl_TessCoord.x * tcDataIn[0].normal;
    tessNorm += gl_TessCoord.y * tcDataIn[1].normal;
    tessNorm += gl_TessCoord.z * tcDataIn[2].normal;
	tessNorm = normalize(tessNorm);
	
	if(gl_TessLevelOuter[0] > 1.0)
	{
		//tri-planar
		vec3 blend = abs(tessNorm);
		blend = normalize(max(blend, 0.00001));
		float b = (blend.x + blend.y + blend.z);
		blend /= vec3(b, b, b);
		
		float scale = 0.05;
		
		vec2 xCoords = tessPos.zy;
		// vec2 xCoords = parallaxMapping(dataIn.position.zy, viewDir, scale, dataIn.normal, 0);
		vec3 xColor = texture(displaceTexture, xCoords * scale).rgb;
		
		vec2 yCoords = tessPos.zx;
		// vec2 yCoords = parallaxMapping(dataIn.position.zx, viewDir, scale, dataIn.normal, 1);
		vec3 yColor = texture(displaceTexture, yCoords * scale).rgb;
		
		vec2 zCoords = tessPos.xy;
		// vec2 zCoords = parallaxMapping(dataIn.position.xy, viewDir, scale, dataIn.normal, 2);
		vec3 zColor = texture(displaceTexture, zCoords * scale).rgb;
		
		vec3 color = xColor * blend.x * 1 + yColor * blend.y * 1 + zColor * blend.z * 1;
		//vec3 normal = xNormal * blend.x * 1 + yNormal * blend.y * 1 + zNormal * blend.z * 1;
		tessPos += -color * 0.5 * tessNorm * (1.0 - tcDataIn[0].tessLevel);
	}
	
	dataOut.normal = normalize(tessNorm);
	//teNormal = normalize(tcDataIn[0].normal +tcDataIn[1].normal+tcDataIn[2].normal);
	//dataOut.normal = normalize(tcDataIn[0].normal +tcDataIn[1].normal+tcDataIn[2].normal);
	
	dataOut.position = tessPos;
	dataOut.fragPosLightSpace = lightMatrix * vec4(tessPos, 1.0);
	
	gl_Position = projection * view * vec4(tessPos, 1.0);
}
