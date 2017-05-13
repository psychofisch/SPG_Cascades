#version 330 core
out vec4 fragColor;

in gsData{
	vec3 position;
	vec3 normal;
	vec4 color;
} gsDataIn;

uniform vec3 lightPos;

void main()
{
	// Diffuse 
	vec3 lightDir = normalize(lightPos - gsDataIn.position);
	float diff = max(dot(lightDir, gsDataIn.normal), 0.0);
	//vec4 diffuse = gsDataIn.color * diff;
	vec4 diffuse = vec4(1.0) * diff;

	fragColor = diffuse;
	//fragColor = vec4(abs(normalize(gsDataIn.normal)), 1.0);
	//fragColor = gsDataIn.color;
	//fragColor = vec4(1.f, 0.0f, 0.0f, 1.0f);
}
