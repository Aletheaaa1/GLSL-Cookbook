#version 460
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform bool is_quad;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

out vec4 FragColor;

vec3 BlinnPhong(vec3 color)
{
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 halfDir = normalize(lightDir + viewDir);

	float LdotN =  max(dot(lightDir, Normal), 0.0);

	vec3 ambient = 0.2 * color;
	vec3 diffuse = LdotN  * color * lightColor;
	vec3 specular = pow(dot(Normal, halfDir), 128) * color * lightColor;

	color = diffuse + ambient + specular;
	return color;
}

void main()
{
	if(!is_quad)
	{
		vec3 texture_color = vec3(0.8f, 0.4f, 0.3f);
		vec3 color = vec4(BlinnPhong(texture_color), 1.0).rgb;

		FragColor = vec4(color, 1.0);
	}
	else
	{
		FragColor = vec4(BlinnPhong(vec3(0.5, 0.5, 0.5)), 1.0);
	}
}