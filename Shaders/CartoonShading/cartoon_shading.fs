#version 460
in vec3 Normal;
in vec3 FragPos;

uniform bool is_quad;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

out vec4 FragColor;

vec3 ToonShade(vec3 color)
{
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);

	vec3 ambient = 0.1 * color;
	vec3 diffuse = max(dot(lightDir, Normal), 0.0) * color * lightColor;
	vec3 specular = pow(dot(Normal, halfDir), 128) * color * lightColor;

	color = diffuse + ambient + specular;
	return color;
}

void main()
{
	if(!is_quad)
		FragColor = vec4(ToonShade(vec3(0.9, 0.7, 0.5)), 1.0);
	else
		FragColor = vec4(ToonShade(vec3(0.5, 0.5, 0.5)), 1.0);
}