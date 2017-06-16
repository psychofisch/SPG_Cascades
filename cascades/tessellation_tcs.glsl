#version 440 core
layout(vertices = 3) out;

in vec3 vPosition[];
out vec3 tcPosition[];

#define ID gl_InvocationID

uniform vec3 cameraPos;

void main()
{
	float TessLevelInner = 1.0f;
    float TessLevelOuter = 1.0f;
	float tessLevel = 1.0f;
	
	tcPosition[ID] = vPosition[ID];
	
	float dist = length(cameraPos - vPosition[ID]);
	dist = min(1.0, dist * 0.03);
	
	tessLevel = 1.0 + ((1.0 - dist) * 4.0);
	
    if (ID == 0) {
        // gl_TessLevelInner[0] = TessLevelInner;
        // gl_TessLevelOuter[0] = TessLevelOuter;
        // gl_TessLevelOuter[1] = TessLevelOuter;
        // gl_TessLevelOuter[2] = TessLevelOuter;
		gl_TessLevelInner[0] = tessLevel;
        gl_TessLevelOuter[0] = tessLevel;
        gl_TessLevelOuter[1] = tessLevel;
        gl_TessLevelOuter[2] = tessLevel;
    }
}
