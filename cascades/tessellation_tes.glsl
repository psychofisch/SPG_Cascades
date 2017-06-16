#version 440 core
layout(triangles, point_mode, ccw) in;

in vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vec3 accum = vec3(0.0);
	accum += gl_TessCoord.x * tcPosition[0];
    accum += gl_TessCoord.y * tcPosition[1];
    accum += gl_TessCoord.z * tcPosition[2];
    tePatchDistance = gl_TessCoord;
    tePosition = accum;
    gl_Position = projection * view * vec4(tePosition, 1.0);
}
