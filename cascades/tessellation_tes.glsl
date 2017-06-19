#version 440 core
layout(triangles/*, point_mode*/, ccw) in;

//in vec3 tcPosition[];
in tcData{
	vec3 position;
	vec3 normal;
	float tessLevel;
} tcDataIn[];

out vData{
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
uniform sampler2D normalMap;

vec3 calcTangents(vec3 vert[3], vec2 uv[3])
{
	vec3 t;

	vec3 edge1 = vert[1] - vert[0];
	vec3 edge2 = vert[2] - vert[0];
	vec2 deltaUV1 = uv[1] - uv[0];
	vec2 deltaUV2 = uv[2] - uv[0];

	float n = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
	float f = 1.f;
	if(abs(n) > 0.001f)
		f = 1.0f / n;

	t.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	t.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	t.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	
	return normalize(t);
}

void main()
{
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
	
	bool test = true;
	// if(gl_TessCoord.x < 0.001 || gl_TessCoord.x > 0.999
		// || gl_TessCoord.y < 0.001 || gl_TessCoord.y > 0.999
		// || gl_TessCoord.z < 0.001 || gl_TessCoord.z > 0.999)
		// test = false;
	
	// if(gl_TessCoord.x == 0 || gl_TessCoord.x == 1.0
		// || gl_TessCoord.y == 0 || gl_TessCoord.y == 1.0
		// || gl_TessCoord.z == 0 || gl_TessCoord.z == 1.0)
		// test = false;
	
	if(test && gl_TessLevelOuter[0] > 1.0)
	{
		//tri-planar
		vec3 blend = abs(tessNorm);
		blend = normalize(max(blend, 0.00001));
		float b = (blend.x + blend.y + blend.z);
		blend /= vec3(b, b, b);
		
		float scale = 0.05;
		
		vec2 xCoords = tessPos.zy;
		vec3 xColor = texture(displaceTexture, xCoords * scale).rgb;
		
		vec2 yCoords = tessPos.zx;
		vec3 yColor = texture(displaceTexture, yCoords * scale).rgb;
		
		vec2 zCoords = tessPos.xy;
		vec3 zColor = texture(displaceTexture, zCoords * scale).rgb;
		
		vec3 color = xColor * blend.x * 1 + yColor * blend.y * 1 + zColor * blend.z * 1;
		tessPos += -color * 1.0 * tessNorm * (1.0 - tcDataIn[0].tessLevel);
	}
	
	dataOut.normal = normalize(tessNorm);
	//teNormal = normalize(tcDataIn[0].normal +tcDataIn[1].normal+tcDataIn[2].normal);
	//dataOut.normal = normalize(tcDataIn[0].normal +tcDataIn[1].normal+tcDataIn[2].normal);
	
	dataOut.position = tessPos;
	dataOut.fragPosLightSpace = lightMatrix * vec4(tessPos, 1.0);
	
	gl_Position = projection * view * vec4(tessPos, 1.0);
}
