#version 440 core
layout(triangles) in;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightMatrix;

in vData{
	mat4 p;
	mat4 v;
	vec3 position;
	vec3 normal;
	vec4 fragPosLightSpace;
} vDataIn[];

out tData{
	mat4 p;
	mat4 v;
	vec3 position;
	vec3 normal;
	vec4 fragPosLightSpace;
} tDataOut;

void main()
{
	tDataOut.p = vDataIn[0].p;
	tDataOut.v = vDataIn[0].v;
	tDataOut.position = vDataIn[0].position;
	tDataOut.normal = vDataIn[0].normal;
	tDataOut.fragPosLightSpace = vDataIn[0].fragPosLightSpace;
	gl_Position = gl_in[0].gl_Position;
}
