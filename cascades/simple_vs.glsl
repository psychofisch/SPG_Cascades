#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;

out vData{
	mat4 p;
	mat4 v;
	vec3 position;
	vec3 normal;
} vDataOut;

void main()
{
	vDataOut.p = projection;
	vDataOut.v = view;
	vDataOut.position = position;
	vDataOut.normal = normal;
	gl_Position = projection * view * vec4(position.x, position.y, position.z, 1.0f);
}
