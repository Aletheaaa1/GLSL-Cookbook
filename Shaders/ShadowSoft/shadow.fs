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

//	泊松盘采样
const int SAMPLE_NUMBER = 50;
const float PI = 3.1415926;
const float SAMPLE_RINGS = 10.0;

//	随机数
float RandomAngel(vec2 randomSeed)
{
	const float a = 12.9898, b = 78.233, c = 43758.5453;
	float dt = dot(randomSeed, vec2(a, b));
	float sn = mod(dt, PI);
	return fract(sin(sn) * c);
}

vec2 poissonDisk[SAMPLE_NUMBER];
void PoissonDiskSamples (const vec2 randomSeed)
{
	float radius = 1.0 / float(SAMPLE_NUMBER);
	float radiusStep = radius;
	float angleStep = 2 * PI * SAMPLE_RINGS / SAMPLE_NUMBER;
	float angle = RandomAngel(randomSeed) * PI * 2;

	for(int i=0; i<SAMPLE_NUMBER; i++)
	{
		poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
		radius += radiusStep;
		angle += angleStep;
	}
}

float FindBlock(vec2 uv, float depth)
{
	float blockAverage = 0.0;
	float count = 0.0;
	// PoissonDiskSamples(uv);
	for(int i=0; i<SAMPLE_NUMBER; i++)
	{
		float blockDepth = texture(depth_texture, uv + sampleRadius * texSize * poissonDisk[i]).r;
		if(depth > blockDepth)
		{
			blockAverage += blockDepth;
			count++;
		}
	}

	return blockAverage / count;
}

float PCSS()
{
	vec4 lightCoord = lightSpaceFragPos.xyzw / lightSpaceFragPos.w;
	lightCoord = lightCoord * 0.5 + 0.5;
	PoissonDiskSamples(lightCoord.xy);

	float currentDepth = lightCoord.z;

	float shadow = 0.0;
	float block = FindBlock(lightCoord.xy, currentDepth);
	float penumbra = (currentDepth - block)/block * 10.0;

	for(int i=0; i<SAMPLE_NUMBER; i++)
	{
		float closestDepth = texture(depth_texture, lightCoord.xy +  penumbra * sampleRadius * texSize * poissonDisk[i]).r;
		if(currentDepth - 0.01 > closestDepth)
		{
			shadow++;
		}
	}
	return shadow / float(SAMPLE_NUMBER);
}

float PCF()
{
	vec4 lightCoord = lightSpaceFragPos.xyzw / lightSpaceFragPos.w;
	lightCoord = lightCoord * 0.5 + 0.5;

	float closetDepth = 0.0;
	float currentDepth = lightCoord.z;
	float shadow = 0.0;

	PoissonDiskSamples(lightCoord.xy);
	for(int i=0; i<SAMPLE_NUMBER; i++)
	{
		float closestDepth = texture(depth_texture, lightCoord.xy +  5 * texSize * poissonDisk[i]).r;
		if(currentDepth - 0.01 > closestDepth)
		{
			shadow++;
		}
	}
	return shadow / float(SAMPLE_NUMBER);
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