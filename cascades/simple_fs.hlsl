#version 330 core
out vec4 fragColor;

in gsData{
	vec3 position;
	vec4 color;
} gsDataIn;

void main()
{
	//fragColor = gsDataIn.color;
	fragColor = vec4(1.f, 0.0f, 0.0f, 1.0f);
}
