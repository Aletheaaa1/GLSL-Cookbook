#version 460
in vec3 Normal;
in vec3 FragPos;

uniform bool is_quad;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform bool is_stencil;

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

	color = diffuse + ambient ;
	return color;
}

void main()
{
		vec3 color;

		if(is_stencil == false)
		{
			color = vec4(BlinnPhong(vec3(0.9, 0.7, 0.5)), 1.0).rgb;
		}
		else
		{
			color = vec3(1.0);
		}

		FragColor = vec4(color, 1.0);
}