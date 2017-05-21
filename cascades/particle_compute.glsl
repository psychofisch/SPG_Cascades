#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in float lifetime;

out feedbackBlock{
	vec3 position;
	float lifetime;
} feedbackOut;

uniform float dt;
uniform float ttl;
uniform float maxLifetime;

void main()
{
	vec3 newPosition = position;
	newPosition.y += 1.0f * dt;
	feedbackOut.position = newPosition;
	feedbackOut.lifetime = lifetime + dt;
}
