#version 440 core
layout(points) in;
layout(triangle_strip, max_vertices = 15) out;

in vData{
	mat4 p;
	mat4 v;
	vec3 position;
	float age;
	vec3 pVelocity;
} vDataIn[];

out Data{
	vec3 position;
	vec3 normal;
	vec4 color;
	mat4 pv;
	float age;
	vec2 uv;
	vec3 velocity;
} gsDataOut;

uniform float maxLifetime;

void build_quad(vec4 position, float size)
{
	gsDataOut.uv = vec2(0,0);
	gl_Position = position + vDataIn[0].p * vec4(-size, -size, 0.0f, 0.0f);    // 1:bottom-left   
	EmitVertex();
	gsDataOut.uv = vec2(1,0);
	gl_Position = position + vDataIn[0].p * vec4(size, -size, 0.0f, 0.0f);    // 2:bottom-right
	EmitVertex();
	gsDataOut.uv = vec2(0,1);
	gl_Position = position + vDataIn[0].p * vec4(-size, size, 0.0f, 0.0f);    // 3:top-left
	EmitVertex();
	gsDataOut.uv = vec2(1,1);
	gl_Position = position + vDataIn[0].p * vec4(size, size, 0.0f, 0.0f);    // 4:top-right
	EmitVertex();
	EndPrimitive();
}

void main()
{
	gsDataOut.age = vDataIn[0].age;
	gsDataOut.velocity = vDataIn[0].pVelocity;
	
	float size = 1.0;
	
	if(gsDataOut.age > 0.6)
		size += (gsDataOut.age - 0.6) * 5.0;
		
	build_quad(gl_in[0].gl_Position, size);
}