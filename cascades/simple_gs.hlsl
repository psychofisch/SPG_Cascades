#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 12) out;
//layout(points, max_vertices = 64) out;

uniform isampler2D vertTable;
uniform sampler3D densityMap;

in vData{
	mat4 p;
	mat4 v;
	vec3 position;//without pv, unlike gl_position
} vDataIn[];

out gsData{
	vec3 position;
	vec4 color;
} gsDataOut;

const vec3 neighbours[8] = vec3[8](vec3(0, 0, 0), vec3(1, 0, 0), vec3(0, 1, 0), vec3(1, 1, 0), vec3(0, 0, 1), vec3(1, 0, 1), vec3(0, 1, 1), vec3(1, 1, 1));
const vec3 edges[12] = vec3[](
	vec3(0.5f, 0, 0),		//0
	vec3(1.f, 0.5f, 0),		//1
	vec3(0.5f, 1.f, 0),		//2
	vec3(0, 0.5f, 0),		//3
	vec3(0.5f, 0, 1.f),		//4
	vec3(1.f, 0.5f, 1.f),	//5
	vec3(0.5f, 1.f, 1.f),	//6
	vec3(0, 0.5f, 1.f),		//7
	vec3(0, 0, 0.5f),		//8
	vec3(1.f, 0, 0.5f),		//9
	vec3(1.f, 1.f, 0.5f),	//10
	vec3(0, 1.f, 0.5f)		//11
);

void build_quad(vec4 position, float size)
{
	gl_Position = position + vDataIn[0].p * vec4(-size, -size, 0.0f, 0.0f);    // 1:bottom-left   
	EmitVertex();
	gl_Position = position + vDataIn[0].p * vec4(size, -size, 0.0f, 0.0f);    // 2:bottom-right
	EmitVertex();
	gl_Position = position + vDataIn[0].p * vec4(-size, size, 0.0f, 0.0f);    // 3:top-left
	EmitVertex();
	gl_Position = position + vDataIn[0].p * vec4(size, size, 0.0f, 0.0f);    // 4:top-right
	EmitVertex();
	EndPrimitive();
}

void buildTriangle(vec3 p0, vec3 p1, vec3 p2)
{
	mat4 pv = vDataIn[0].p * vDataIn[0].v;

	//gsDataOut.color = vec4(1.0f, 0, 0, 1.0f);
	gl_Position = gl_in[0].gl_Position + pv * vec4(p0, 1.0f);
	EmitVertex();

	//gsDataOut.color = vec4(0, 1.0f, 0, 1.0f);
	gl_Position = gl_in[0].gl_Position + pv * vec4(p1, 1.0f);
	EmitVertex();

	//gsDataOut.color = vec4(0, 0, 1.0f, 1.0f);
	gl_Position = gl_in[0].gl_Position + pv * vec4(p2, 1.0f);
	EmitVertex();

	EndPrimitive();
}

void main() {
	//gsDataOut.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec3 texSize = textureSize(densityMap, 0);
	vec3 texCoords = ((vDataIn[0].position / texSize)) + .5f;
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
		{
			lookupIndex |= 1 << i;
		}
	}

	vec3 posColor = vDataIn[0].position / texSize;
	gsDataOut.color = vec4(posColor.x, posColor.y, posColor.z, 1.0f);

	for (int i = 0; i < 16; i += 3)
	{
		if (texture(vertTable, vec2(float(i) / 16, 1.0 - (float(lookupIndex) / 255))).r != -1)
		{
			int points[3];
			points[0] = texture(vertTable, vec2(float(i) / 16, 1.0 - (float(lookupIndex) / 255))).r;
			points[1] = texture(vertTable, vec2(float(i+1) / 16, 1.0 - (float(lookupIndex) / 255))).r;
			points[2] = texture(vertTable, vec2(float(i+2) / 16, 1.0 - (float(lookupIndex) / 255))).r;

			/*gsDataOut.color = vec4(1.0f, 0, 0, 1.0f);
			gl_Position = gl_in[0].gl_Position;
			EmitVertex();
			EndPrimitive();*/
			buildTriangle(edges[points[0]] * 2.0f, edges[points[1]] * 2.0f, edges[points[2]] * 2.0f);
		}
		/*else
		{
			gsDataOut.color = vec4(0, 1.0f, 0, 1.0f);
			gl_Position = gl_in[0].gl_Position;
			EmitVertex();
			EndPrimitive();
			break;
		}*/
	}

	//if (lookupIndex == 0 && lookupIndex < 255)
	//{
	//	gsDataOut.color = vec4(lookupIndex/255.f, 1.0f, 0.0f, 1.0f);
	//	build_quad(gl_in[0].gl_Position, 0.5f);
	//	/*gl_Position = gl_in[0].gl_Position + vDataIn[0].pv * vec4(offset, 1.0f);
	//	EmitVertex();
	//	EndPrimitive();*/
	//}
}