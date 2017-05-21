#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in float lifetime;

out feedbackBlock{
	vec3 position;
	float lifetime;
} feedbackOut;

// struct EmitterStruct{
	// vec3 position;
	// vec3 normal;
// };

uniform float dt;
uniform float maxLifetime;
uniform float emitters[10 * 6];
uniform int activeEmitters;

float randFunction(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	if(lifetime > 0.0f)
	{
		vec3 newPosition = position;
	
		newPosition.y += 1.0f * dt;
		feedbackOut.position = newPosition;
		feedbackOut.lifetime = lifetime - dt;
	}
	else
	{
		if(activeEmitters > 0)
		{
			float r = randFunction(position.xy);
			if(r < 0.01f)
			{
				//feedbackOut.position = emitters[int(round(r * activeEmitters))].position;
				feedbackOut.position = vec3(emitters[0], emitters[1], emitters[2]);
				feedbackOut.lifetime = maxLifetime;
			}
		}
	}
}
