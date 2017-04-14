#version 330 core
layout(points) in;
//layout(triangle_strip, max_vertices = 5) out;
layout(points, max_vertices = 1) out;

uniform sampler3D densityMap;

in vData{
	mat4 pv;
} vDataIn[];

out vec4 gsColor;

void build_quad(vec4 position)
{
	gl_Position = position + vDataIn[0].pv * vec4(-0.2f, -0.2f, 0.0f, 0.0f);    // 1:bottom-left   
	EmitVertex();
	gl_Position = position + vDataIn[0].pv * vec4(0.2f, -0.2f, 0.0f, 0.0f);    // 2:bottom-right
	EmitVertex();
	gl_Position = position + vDataIn[0].pv * vec4(-0.2f, 0.2f, 0.0f, 0.0f);    // 3:top-left
	EmitVertex();
	gl_Position = position + vDataIn[0].pv * vec4(0.2f, 0.2f, 0.0f, 0.0f);    // 4:top-right
	EmitVertex();
	EndPrimitive();
}

void main() {
	//build_quad(gl_in[0].gl_Position);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();

	gsColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}