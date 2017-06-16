#version 440 core
layout(triangles/*, point_mode*/, ccw) in;

//in vec3 tcPosition[];
in tcData{
	vec3 position;
	vec3 normal;
} tcDataIn[];

out vec3 tePosition;
out vec3 teNormal;
out vec3 tePatchDistance;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vec3 tessPos = vec3(0.0);
	vec3 tessNorm = vec3(0.0);
	float offset = 0.0f;
	offset = 0.8165 - length(gl_TessCoord - vec3(0.3333));
	tessPos += gl_TessCoord.x * tcDataIn[0].position;
    tessPos += gl_TessCoord.y * tcDataIn[1].position;
    tessPos += gl_TessCoord.z * tcDataIn[2].position;
	tessPos += offset * 1.0f * tcDataIn[0].normal;
    tePatchDistance = gl_TessCoord;
    tePosition = tessPos;
    gl_Position = projection * view * vec4(tePosition, 1.0);
	
	tessNorm += gl_TessCoord.x * tcDataIn[0].normal;
    tessNorm += gl_TessCoord.y * tcDataIn[1].normal;
    tessNorm += gl_TessCoord.z * tcDataIn[2].normal;
	teNormal = normalize(tessNorm);
}
