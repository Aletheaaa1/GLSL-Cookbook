#version 460
in vec2 TexCoord;

uniform sampler2D gPosition_texture;
uniform sampler2D gNormal_texture;
uniform sampler2D gColor_texture;
uniform sampler2D noise_texture;
uniform vec3 kernel_samples[64];
uniform mat4 projection;

out vec4 FragColor;

void main()
{
	vec3 fragPos = texture(gPosition_texture, TexCoord).rgb;
	vec3 normal = texture(gNormal_texture, TexCoord).rgb;
	vec4 color = texture(gColor_texture, TexCoord);

	//	TBN
	vec2 randomVecSize = vec2(800.0 / 4.0, 600.0 / 4.0);
	vec3 randomVec = texture(noise_texture, TexCoord * randomVecSize).xyz;
	//vec3 randomVec = vec3(1, 1, 1);
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float ao = 0.0;
	for(int i = 0; i < 64; i++)
	{
		vec3 kernel_sample = TBN * kernel_samples[i];
		//	kernel_sample.z 为采样点深度
		kernel_sample = kernel_sample + fragPos;

		//	获取采样点对应的屏幕UV坐标
		vec4 samplerP = projection * vec4(kernel_sample, 1.0);
		samplerP.xyz /= samplerP.w;
		samplerP.xyz = samplerP.xyz * 0.5 + 0.5;

		float currentScreenDepth = texture(gPosition_texture, samplerP.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, 1.0 / abs(kernel_sample.z - currentScreenDepth));

		if( currentScreenDepth > kernel_sample.z)
		{
			ao += rangeCheck * 1.0;
		}
	}

	FragColor = vec4(vec3(1 - ao/64.0), 1.0);
}