#version 440 core
out vec4 fragColor;

in float pLifetime;
in vec3 pVelocity;

uniform float maxLifetime;

void main()
{
	if(pLifetime <= 0.0f)
		discard;
		
	//fragColor = vec4(1.f);
	fragColor = vec4(pVelocity, 1.f);
}