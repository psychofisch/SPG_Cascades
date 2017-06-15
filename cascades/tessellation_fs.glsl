#version 440 core
out vec4 fragColor;
in vec3 tePosition;
in vec3 tePatchDistance;

void main()
{
	//fragColor = vec4(1.0, 1.0 , 0.0, 1.0);
	fragColor = vec4(tePosition, 1.0);
}
