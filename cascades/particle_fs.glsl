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
	float age = pLifetime / maxLifetime;
	vec3 fireColor = mix(vec3(0,0.2,1), vec3(1,0.5,0), 1.0f - pow(age, 2));
	fragColor = vec4(fireColor, 1.f);
}