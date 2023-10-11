#version 460
in vec2 TexCoord;

uniform sampler2D ssao_texture;

out vec4 FragColor;

void main()
{
	vec2 texelSize = 1.0 / textureSize(ssao_texture, 0);
	float result = 0.0;
	for( int x = -2; x < 2; x++)
	{
		for( int y = -2; y < 2; y++)
		{
			vec2 offset = vec2(x, y) * texelSize + TexCoord;
			result += texture(ssao_texture, offset).r;
		}
	}

	result /= 16.0;
	FragColor = vec4(vec3(result), 1.0);
}