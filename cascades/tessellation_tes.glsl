#version 440 core
layout(triangles/*, point_mode*/, ccw) in;

//in vec3 tcPosition[];
in tcData{
	vec3 position;
	vec3 normal;
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

void main()
{
	dataOut.p = projection;
	dataOut.v = view;

	vec3 tessPos = vec3(0.0);
	vec3 tessNorm = vec3(0.0);
	float offset = 0.0f;
	offset = 0.8165 - length(gl_TessCoord - vec3(0.3333));
	tessPos += gl_TessCoord.x * tcDataIn[0].position;
    tessPos += gl_TessCoord.y * tcDataIn[1].position;
    tessPos += gl_TessCoord.z * tcDataIn[2].position;
	//tessPos += offset * 1.0f * tcDataIn[0].normal;
    //tePatchDistance = gl_TessCoord;
    dataOut.position = tessPos;
    gl_Position = projection * view * vec4(tessPos, 1.0);
	
	tessNorm += gl_TessCoord.x * tcDataIn[0].normal;
    tessNorm += gl_TessCoord.y * tcDataIn[1].normal;
    tessNorm += gl_TessCoord.z * tcDataIn[2].normal;
	dataOut.normal = normalize(tessNorm);
	//teNormal = normalize(tcDataIn[0].normal +tcDataIn[1].normal+tcDataIn[2].normal);
	//dataOut.normal = normalize(tcDataIn[0].normal +tcDataIn[1].normal+tcDataIn[2].normal);

	dataOut.fragPosLightSpace = lightMatrix * vec4(tessPos, 1.0);
}
