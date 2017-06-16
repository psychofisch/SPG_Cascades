#version 440 core
out vec4 fragColor;

in vec3 tePosition;
in vec3 tePatchDistance;
in vec3 teNormal;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
	//fragColor = vec4(1.0, 1.0 , 0.0, 1.0);
	//fragColor = vec4(tePosition, 1.0);
	vec3 color = vec3(1.0);
	
	vec3 lightColor = vec3(1.0);
	vec3 normal = teNormal;
	
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor * color;
	
	vec3 lightDir = normalize(lightPos - tePosition);
	vec3 viewDir = normalize(cameraPos - tePosition);
	float diff = max(dot(lightDir, normal), 0.0);
	//vec4 diffuse = dataIn.color * diff;
	vec3 diffuse = diff * lightColor * color;

	// Specular
	float specularStrength = 0.3;
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = specularStrength * spec * vec3(1.0)/** lightColor*/;  
	
	vec3 lighting = (diffuse + specular) + ambient;
	fragColor = vec4(lighting, 1.0);
}
