#version 440 core
layout(vertices = 3) out;

//in vec3 vPosition[];
in vData{
	vec3 position;
	vec3 normal;
} vDataIn[];

//out vec3 tcPosition[];
out tcData{
	vec3 position;
	vec3 normal;
} tcDataOut[];

#define ID gl_InvocationID

uniform vec3 cameraPos;

void main()
{
	float TessLevelInner = 1.0f;
    float TessLevelOuter = 1.0f;
	float tessLevel = 1.0f;
	
	tcDataOut[ID].position = vDataIn[ID].position;
	tcDataOut[ID].normal = vDataIn[ID].normal;
	
	float dist = length(cameraPos - vDataIn[ID].position);
	dist = min(1.0, pow(dist * 0.05, 4));
	
	tessLevel = 1.0 + ((1.0 - dist) * 8.0);
	tessLevel = 1.0;
	// if(dist < 1.0)
		// tessLevel = 8.0;
	// else
		// tessLevel = 1.0f;
	
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
