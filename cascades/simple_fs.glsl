#version 330 core
out vec4 fragColor;

in Data{
	vec3 position;
	vec3 normal;
	vec4 color;
} dataIn;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
	vec3 lightColor = vec3(0, 1.0, 0);
	vec3 normal = dataIn.normal;

	// Ambient
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	
	// Diffuse 
	vec3 lightDir = normalize(lightPos - dataIn.position);
	vec3 viewDir = normalize(cameraPos - dataIn.position);
	float diff = max(dot(lightDir, normal), 0.0);
	//vec4 diffuse = dataIn.color * diff;
	vec3 diffuse = diff * lightColor;

	// Specular
	float specularStrength = 0.3;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = specularStrength * spec * vec3(1.0)/** lightColor*/;  
	
	vec3 lighting = diffuse + specular;
	fragColor = vec4(lighting, 1.0);
	//fragColor = vec4(abs(normalize(dataIn.normal)), 1.0);
	//fragColor = dataIn.color;
	//fragColor = vec4(1.f, 0.0f, 0.0f, 1.0f);
}
