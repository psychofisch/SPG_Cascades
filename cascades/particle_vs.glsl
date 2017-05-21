#version 440 core
layout(location = 0) in vec3 position;
layout(location = 1) in float lifetime;

out float pLifetime;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_PointSize = 5.f;
	gl_Position = projection * view * vec4(position.x, position.y, position.z, 1.0f);
	pLifetime = lifetime;
}