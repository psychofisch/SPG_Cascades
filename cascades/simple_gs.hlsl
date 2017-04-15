#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
//layout(points, max_vertices = 1) out;

uniform sampler3D densityMap;

in vData{
	mat4 pv;
	vec3 position;
} vDataIn[];

out gsData{
	vec3 position;
	vec4 color;
} gsDataOut;

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
	//gsDataOut.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec3  texCoords = ((vDataIn[0].position / textureSize(densityMap, 0)) * 2.f) - 1.f;
	float density = texture(densityMap, texCoords).r;
	//gsDataOut.color = vec4(texture(densityMap, vDataIn[0].position / textureSize(densityMap, 0)).r, 0.0f, 0.0f, 1.0f);
	gsDataOut.position = vDataIn[0].position;
	
	if (density > 0.0f)
	{
		build_quad(gl_in[0].gl_Position);
		//gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		EndPrimitive();
	}
}