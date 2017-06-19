#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 lightMatrix;

out vData{
	vec3 position;
	vec3 normal;
	vec4 fragPosLightSpace;
} vDataOut;

void main()
{
	vDataOut.position = position;
	vDataOut.normal = normal;
	vDataOut.fragPosLightSpace = lightMatrix * vec4(position, 1.0);
	gl_Position = projection * view * vec4(position, 1.0f);
}
