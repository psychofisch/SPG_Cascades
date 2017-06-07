#version 440 core
out vec4 fragColor;

in Data{
	vec3 position;
	vec3 normal;
	vec4 color;
	mat4 pv;
} dataIn;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform sampler2D diffuseTexture;

void main()
{
	vec3 lightColor = vec3(1.0);
	vec3 normal = dataIn.normal;

	//tri-planar
	vec3 blend = abs(dataIn.normal);
	blend = normalize(max(blend, 0.00001));
	float b = (blend.x + blend.y + blend.z);
	blend /= vec3(b, b, b);
	
	vec3 xColorCode = vec3(1.0, 0, 0);
	vec3 yColorCode = vec3(0, 1.0, 0);
	vec3 zColorCode = vec3(0, 0, 1.0);
	
	float scale = 0.1;
	float colorFactor = 1.0;
	
	vec2 xCoords = dataIn.position.zy;
	vec3 xColor = texture(diffuseTexture, xCoords * scale).rgb * max(xColorCode, colorFactor);
	
	vec2 yCoords = dataIn.position.zx;
	vec3 yColor = texture(diffuseTexture, yCoords * scale).rgb * max(yColorCode, colorFactor);
	
	vec2 zCoords = dataIn.position.xy;
	vec3 zColor = texture(diffuseTexture, zCoords * scale).rgb * max(zColorCode, colorFactor);
	
	vec3 color = xColor * blend.x * 1 + yColor * blend.y * 1 + zColor * blend.z * 1;
	
	// Ambient
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	
	// Diffuse 
	vec3 lightDir = normalize(lightPos - dataIn.position);
	vec3 viewDir = normalize(cameraPos - dataIn.position);
	float diff = max(dot(lightDir, normal), 1.0);
	//vec4 diffuse = dataIn.color * diff;
	vec3 diffuse = diff * lightColor * color;

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
