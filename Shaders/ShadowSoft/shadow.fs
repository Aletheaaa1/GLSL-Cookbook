#version 460
in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
in vec4 lightSpaceFragPos;

out vec4 FragColor;

uniform sampler2D depth_texture;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 color;

vec2 texSize = 1.0 / textureSize(depth_texture, 0);
const int sampleRadius = 3;

float FindBlock(vec2 uv, float depth)
{
	float blockAverage = 0.0;
	float count = 0.0;
	for(int x = -sampleRadius; x<=sampleRadius; x++)
	{
		for(int y=-sampleRadius; y<=sampleRadius; y++)
		{
			float blockDepth = texture(depth_texture, uv + sampleRadius * texSize * vec2(x, y)).r;
			if(depth > blockDepth)
			{
				blockAverage += blockDepth;
				count++;
			}
		}
	}

	return blockAverage / count;
}

float PCSS()
{
	vec4 lightCoord = lightSpaceFragPos.xyzw / lightSpaceFragPos.w;
	lightCoord = lightCoord * 0.5 + 0.5;

	float currentDepth = lightCoord.z;

	float shadow = 0.0;
	float block = FindBlock(lightCoord.xy, currentDepth);
	float penumbra = (currentDepth - block)/block * 3.0;

	for(int x = -sampleRadius; x<=sampleRadius; x++)
	{
		for(int y=-sampleRadius; y<=sampleRadius; y++)
		{
			float closestDepth = texture(depth_texture, lightCoord.xy +  penumbra * sampleRadius * texSize * vec2(x, y)).r;
			if(currentDepth - 0.01 > closestDepth)
			{
				shadow++;
			}
		}
	}

	return shadow / 49.0;
}

float PCF()
{
	vec4 lightCoord = lightSpaceFragPos.xyzw / lightSpaceFragPos.w;
	lightCoord = lightCoord * 0.5 + 0.5;

	float closetDepth = 0.0;
	float currentDepth = lightCoord.z;
	float shadow = 0.0;

	for(int x = -sampleRadius; x < sampleRadius; ++x)
	{
		for(int y = -sampleRadius; y < sampleRadius; ++y)
		{
			closetDepth = texture(depth_texture, lightCoord.xy + 2.0 * vec2(x, y) * texSize).r;
			shadow += currentDepth - 0.01 > closetDepth ? 1.0 : 0.0;
		}
    }
	shadow = shadow / 49.0;
	return shadow;
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

	float shadowPCF = PCF();
	float shadowPCSS = PCSS();

	FragColor = vec4(ambient + (1.0 - shadowPCSS) * (diffuse + specular), 1.0);
//	FragColor = vec4(vec3(1.0-shadow), 1.0);
}