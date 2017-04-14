#version 330 core
out vec4 color;

in vec4 gsColor;

void main()
{
	color = gsColor;
}
