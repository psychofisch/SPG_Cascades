#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vData{
	mat4 pv;
} vDataOut;

void main()
{
	vDataOut.pv = projection * view;
	gl_Position = vDataOut.pv * vec4(position.x, position.y, position.z, 1.0f);
}
