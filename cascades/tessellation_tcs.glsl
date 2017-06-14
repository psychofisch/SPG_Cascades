#version 440 core
layout(vertices = 3) out;

in vec3 vPosition[];
out vec3 tcPosition[];

const float TessLevelInner = 4.0f;
const float TessLevelOuter = 4.0f;

#define ID gl_InvocationID

void main()
{
	tcPosition[ID] = vPosition[ID];
    if (ID == 0) {
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }
}
