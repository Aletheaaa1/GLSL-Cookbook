#version 460
in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
in vec4 lightSpaceFragPos;

out vec4 FragColor;

uniform sampler2DShadow depth_texture;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 color;

float GetShadow()
{
	vec4 lightCoord = lightSpaceFragPos.xyzw / lightSpaceFragPos.w;
	lightCoord = lightCoord * 0.5 + 0.5;

	float closetDepth = 0;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(-1, -1)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(-1, 1)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(1, -1)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(1, 1)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(-1, 0)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(0, 1)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(0, -1)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(1, 0)).r;
	closetDepth += textureProjOffset(depth_texture, lightCoord, ivec2(0, 0)).r;
	closetDepth = closetDepth / 9.0;

	float currentDepth = lightCoord.z;

	if(currentDepth - 0.05 > closetDepth)
	{
		return 1.0;
	}

	return 0.0;
}

void main()
{
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 halfDir = normalize(lightDir + viewDir);

	vec3 ambient = vec3(0.3f) * color;

	float diffuseRation = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseRation * lightColor * color;

	float specularRation = pow(max(dot(normal, halfDir), 0.0), 128);
	vec3 specular = specularRation * lightColor * color;

	float shadow = GetShadow();

	FragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0);
//	FragColor = vec4(vec3(1.0-shadow), 1.0);
}