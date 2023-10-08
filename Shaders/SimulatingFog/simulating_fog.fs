#version 460
in vec3 Normal;
in vec3 FragPos;

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

	color = diffuse + ambient ;
	return color;
}

void main()
{
	if(!is_quad)
	{
		vec3 color = vec4(BlinnPhong(vec3(0.9, 0.7, 0.5)), 1.0).rgb;

		//	Fog Calculate
		const float fogMax = 30.0;
		const float fogMin = 1.0;
		const vec3 fogColor = vec3(0.5);

		float dist = length(FragPos.xyz);
		float fogFactor = (fogMax - dist) / (fogMax - fogMin);
		fogFactor = clamp(fogFactor, 0.0, 1.0);
		color = mix(fogColor, color, fogFactor);

		FragColor = vec4(color, 1.0);
	}
	else
	{
		FragColor = vec4(BlinnPhong(vec3(0.5, 0.5, 0.5)), 1.0);
	}
}