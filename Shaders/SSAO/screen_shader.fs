#version 460
in vec2 TexCoord;

uniform sampler2D gPosition_texture;
uniform sampler2D gNormal_texture;
uniform sampler2D gColor_texture;
uniform sampler2D ssao_texture;

uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

out vec4 FragColor;

void main()
{
	vec3 normal = texture(gNormal_texture, TexCoord).rgb;
	vec3 fragPos = texture(gPosition_texture, TexCoord).rgb;
	vec3 color = texture(gColor_texture, TexCoord).rgb;
	float ssao = texture(ssao_texture, TexCoord).r;

	vec3 lightPosV = (projection * vec4(lightPos, 1.0)).xyz;
	vec3 viewPosV = (projection * vec4(cameraPos, 1.0)).xyz;

	vec3 lightDir = normalize(lightPosV - fragPos);
	vec3 viewDir = normalize(viewPosV - fragPos);
	vec3 halfDir = normalize(viewDir + lightDir);

	vec3 ambient = vec3(0.2 * ssao);
	vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor * color;
	vec3 specular = pow(max(dot(normal, halfDir), 0), 32) * lightColor * color;

	color = ambient + diffuse + specular;
	FragColor = vec4(vec3(color), 1.0);
}