#version 440 core
out vec4 fragColor;

in float pLifetime;

uniform float maxLifetime;

void main()
{
	if(pLifetime > maxLifetime)
		discard;
		
	fragColor = vec4(1.f);
}