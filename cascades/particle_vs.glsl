#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float lifetime;

out float pLifetime;
out vec3 pVelocity;

uniform mat4 projection;
uniform mat4 view;
uniform float maxLifetime;

void main()
{
	//gl_PointSize = 10.f * max(0.2, (lifetime / maxLifetime));
	gl_PointSize = 5.f;
	gl_Position = projection * view * vec4(position.x, position.y, position.z, 1.0f);
	pLifetime = lifetime;
	pVelocity = velocity;
}
