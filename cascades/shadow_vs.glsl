#version 330 core
  
layout (location = 0) in vec3 position;

uniform mat4 transform;
uniform mat4 lightMatrix;

void main()
{
    gl_Position = lightMatrix * vec4(position, 1.0f);
}
