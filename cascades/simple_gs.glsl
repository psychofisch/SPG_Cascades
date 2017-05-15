#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 32) out;
//layout(points, max_vertices = 64) out;

uniform isampler2D vertTable;
uniform isampler2D edgeTable;
uniform sampler3D densityMap;
uniform float densityThreshold;

in vData{
	mat4 p;
	mat4 v;
	vec3 position;//without pv, unlike gl_position
} vDataIn[];

out Data{
	vec3 position;
	vec3 normal;
	vec4 color;
} gsDataOut;

const vec3 neighbours[8] = vec3[](
	vec3(0, 0, 0),
	vec3(1, 0, 0),
	vec3(1, 0, 1),
	vec3(0, 0, 1),
	vec3(0, 1, 0),
	vec3(1, 1, 0),
	vec3(1, 1, 1),
	vec3(0, 1, 1)
);

const vec3 edges[12] = vec3[](
	vec3(0.5f, 0, 0),		//0
	vec3(1.f, 0, 0.5f),		//1
	vec3(0.5f, 0, 1.f),		//2
	vec3(0, 0, 0.5f),		//3
	vec3(0.5f, 1.f, 0),		//4
	vec3(1.f, 1.f, 0.5f),	//5
	vec3(0.5f, 1.f, 1.f),	//6
	vec3(0, 1.f, 0.5f),		//7
	vec3(0, 0.5f, 0),		//8
	vec3(1.f, 0.5f, 0),		//9
	vec3(1.f, 0.5f, 1.f),	//10
	vec3(0, 0.5f, 1.f)		//11
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

vec4 decideColor(int i)
{
	if (i >= 0 && i < 4)
		return vec4(1.f, 0, 0, 1.0f);
	else if (i >= 4 && i < 8)
		return vec4(0, 1.f, 0, 1.0f);
	else if (i >= 8)
		return vec4(0, 0, 1.f, 1.0f);
	else
		return vec4(1.f, 1.f, 1.f, 1.f);
}

void buildTriangle(vec3 p0, vec3 p1, vec3 p2)
{
	mat4 pv = vDataIn[0].p * vDataIn[0].v;

	//gsDataOut.color = decideColor(p0);
	gl_Position = gl_in[0].gl_Position + pv * vec4(p0, 1.0f);
	EmitVertex();
	
	//gsDataOut.color = decideColor(p1);
	gl_Position = gl_in[0].gl_Position + pv * vec4(p1, 1.0f);
	EmitVertex();

	//gsDataOut.color = decideColor(p2);
	gl_Position = gl_in[0].gl_Position + pv * vec4(p2, 1.0f);
	EmitVertex();

	EndPrimitive();
}

vec3 calculateNormals(vec3 p1, vec3 p2, vec3 p3)
{
	vec3 normal;

	vec3 u = p2 - p1;
	vec3 v = p3 - p1;

	normal.x = (u.y * v.z) - (u.z * v.y);
	normal.y = (u.z * v.x) - (u.x * v.z);
	normal.z = (u.x * v.y) - (u.y * v.x);

	return normalize(-normal);
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

		if (density > densityThreshold)
		{
			lookupIndex |= 1 << i;
		}
	}

	vec3 posColor = vDataIn[0].position / texSize;
	gsDataOut.color = vec4(posColor.x, posColor.y, posColor.z, 1.0f);
	gsDataOut.color = vec4(0, 1.f, 0, 1.0f);
	vec3 normals;

	for (int i = 0; i < 16; i += 3)
	{
		if (texture(vertTable, vec2(float(i) / 16, 1.0 - (float(lookupIndex) / 255))).r != -1)
		{
			int points[3];
			points[0] = texture(vertTable, vec2(float(i) / 16, 1.0 - (float(lookupIndex) / 255))).r;
			points[1] = texture(vertTable, vec2(float(i + 1) / 16, 1.0 - (float(lookupIndex) / 255))).r;
			points[2] = texture(vertTable, vec2(float(i + 2) / 16, 1.0 - (float(lookupIndex) / 255))).r;

			gsDataOut.color = vec4(1.0f, 0, 0, 1.0f);
			normals = calculateNormals(edges[points[0]], edges[points[1]], edges[points[2]]);
			gsDataOut.normal = normals;
			gsDataOut.color = vec4(abs(normals), 1.0);
			buildTriangle(edges[points[0]], edges[points[1]], edges[points[2]]);
		}
		else
			break;
	}
}