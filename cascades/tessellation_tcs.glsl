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
	float tessLevel;
} tcDataOut[];

#define ID gl_InvocationID

uniform vec3 cameraPos;
uniform int displaceMode;

void main()
{
	float TessLevelInner = 1.0f;
    float TessLevelOuter = 1.0f;
	float tessLevel = 2.0f;
	
	tcDataOut[ID].position = vDataIn[ID].position;
	tcDataOut[ID].normal = vDataIn[ID].normal;
	
	if(displaceMode == 0)
	{
		float dist = length(cameraPos - vDataIn[ID].position);
		dist = min(1.0, pow(dist * 0.05, 4));
		
		tessLevel = 1.0 + ((1.0 - dist) * 8.0);
		tcDataOut[ID].tessLevel = dist;
		// if(dist < 1.0)
			// tessLevel = 16.0;
		// else
			// tessLevel = 1.0f;
			
		//tessLevel = 1.0;
	}
	
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
