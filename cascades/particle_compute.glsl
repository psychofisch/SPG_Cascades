#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float lifetime;

out feedbackBlock{
	vec3 position;
	vec3 velocity;
	float lifetime;
} feedbackOut;

// struct EmitterStruct{
	// vec3 position;
	// vec3 normal;
// };

uniform float dt;
uniform float maxLifetime;
uniform float emitters[10 * 6];
uniform uint activeEmitters;

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
			float r = randFunction(vec2(gl_VertexID, dt * 100));
			if(r < 0.01f)
			{
				//int index = int(floor(r * activeEmitters)) * 6;
				uint index = (gl_VertexID%activeEmitters) * 6;
				//index = 0;
				feedbackOut.position = vec3(emitters[index], emitters[index + 1], emitters[index + 2]);
				//feedbackOut.position = vec3(0);
				feedbackOut.velocity = vec3(emitters[index + 3], emitters[index + 4], emitters[index + 5]);
				//feedbackOut.velocity = vec3(0);
				//feedbackOut.position = vec3(emitters[0], emitters[1], emitters[2]);
				//feedbackOut.lifetime = maxLifetime;
				feedbackOut.lifetime = 2.0f;
			}
		}
	}
}
