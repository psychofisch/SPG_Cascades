#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;

in vData{
	mat4 p;
mat4 v;
} vDataIn[];

void build_house(vec4 position)
{
	gl_Position = (position + vec4(-0.2f, -0.2f, 0.0f, 0.0f));    // 1:bottom-left   
	EmitVertex();
	gl_Position = position + vec4(0.2f, -0.2f, 0.0f, 0.0f);    // 2:bottom-right
	EmitVertex();
	gl_Position = position + vec4(-0.2f, 0.2f, 0.0f, 0.0f);    // 3:top-left
	EmitVertex();
	gl_Position = position + vec4(0.2f, 0.2f, 0.0f, 0.0f);    // 4:top-right
	EmitVertex();
	EndPrimitive();
}

void main() {
	build_house(gl_in[0].gl_Position);
}