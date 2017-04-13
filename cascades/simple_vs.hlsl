#version 330 core
layout(location = 0) in vec2 position;

uniform mat4 projection;
uniform mat4 view;

out vData{
	mat4 p;
	mat4 v;
} vDataOut;

void main()
{
	gl_Position = projection * view * vec4(position.x, position.y, -5.0f, 1.0f);
	vDataOut.p = projection;
	vDataOut.v = view;
}
