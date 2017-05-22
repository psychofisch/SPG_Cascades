#version 440 core
out vec4 fragColor;

uniform float maxLifetime;
uniform sampler2D smokeTexture;

in Data{
	vec3 position;
	vec3 normal;
	vec4 color;
	mat4 pv;
	float lifetime;
	vec2 uv;
	vec3 velocity;
} gsDataIn;

void main()
{
	if(gsDataIn.lifetime <= 0.0f)
		discard;
		
	//fragColor = vec4(1.f);
	float age = 1.0 - gsDataIn.lifetime / maxLifetime;
	vec3 fireColor = mix(vec3(1,1,0), vec3(1,0,0), pow(age + 0.2, 2));
	vec4 smoke = texture(smokeTexture, gsDataIn.uv).rgba;
	fragColor = smoke * vec4(fireColor, 1.0);
	
	if(age > 0.8)
		fragColor.a -= (age - 0.8) * 5.0;
	//fragColor = vec4(color, 0.9f);
}