#version 330 core
layout(points) in;
//layout(triangle_strip, max_vertices = 5) out;
layout(points, max_vertices = 4) out;

uniform sampler3D densityMap;

in vData{
	mat4 pv;
	vec3 position;//without pv, unlike gl_position
} vDataIn[];

out gsData{
	vec3 position;
	vec4 color;
} gsDataOut;

const vec3 neighbours[8] = vec3[8](vec3(0,0,0), vec3(1,0,0), vec3(0,1,0), vec3(1,1,0), vec3(0, 0, 1), vec3(1, 0, 1), vec3(0, 1, 1), vec3(1, 1, 1));

void build_quad(vec4 position, float size)
{
	gl_Position = position + vDataIn[0].pv * vec4(-size, -size, 0.0f, 0.0f);    // 1:bottom-left   
	EmitVertex();
	gl_Position = position + vDataIn[0].pv * vec4(size, -size, 0.0f, 0.0f);    // 2:bottom-right
	EmitVertex();
	gl_Position = position + vDataIn[0].pv * vec4(-size, size, 0.0f, 0.0f);    // 3:top-left
	EmitVertex();
	gl_Position = position + vDataIn[0].pv * vec4(size, size, 0.0f, 0.0f);    // 4:top-right
	EmitVertex();
	EndPrimitive();
}

void main() {
	//gsDataOut.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec3 texCoords = ((vDataIn[0].position / textureSize(densityMap, 0))) + .5f;
	float density = texture(densityMap, texCoords).r;
	//gsDataOut.color = vec4(texture(densityMap, vDataIn[0].position / textureSize(densityMap, 0)).r, 0.0f, 0.0f, 1.0f);
	gsDataOut.position = vDataIn[0].position;

	int lookupIndex = 0;
	for (int i = 0; i < 8; ++i)
	{
		vec3 offset = neighbours[i];

		vec3 texCoords = (((vDataIn[0].position + offset) / textureSize(densityMap, 0))) + .5f;
		density = texture(densityMap, texCoords).r;

		if (density > 0.f)
			lookupIndex |= 1 << i;

		if(density > 0.f)
			gsDataOut.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		else
			gsDataOut.color = vec4(1.0f, 0.0f, 1.0f, 1.0f);

		if (lookupIndex > 0 && lookupIndex < 255)
		{
			gl_Position = gl_in[0].gl_Position + vDataIn[0].pv * vec4(offset, 1.0f);
			EmitVertex();
			EndPrimitive();
		}
	}
	
	//if (density > 0.0f)
	//{
	//	gsDataOut.color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	//	gl_Position = gl_in[0].gl_Position;
	//	EmitVertex();
	//	EndPrimitive();
	//	//build_quad(gl_in[0].gl_Position, 0.2f);
	//}
	//else {
	//	gsDataOut.color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	//	gl_Position = gl_in[0].gl_Position;
	//	EmitVertex();
	//	EndPrimitive();
	//	//build_quad(gl_in[0].gl_Position, 0.05f);
	//}
}