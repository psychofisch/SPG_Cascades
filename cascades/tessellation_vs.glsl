#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

//out vec3 vPosition;
out vData{
	vec3 position;
	vec3 normal;
} vDataOut;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vDataOut.position = position;
	vDataOut.normal = normal;
}
