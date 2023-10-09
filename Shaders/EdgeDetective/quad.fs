#version 460
in vec2 TexCoord;
uniform sampler2D screen_texture;
out vec4 FragColor;

//	¡¡∂»º∆À„
const vec3 lume = vec3(0.2126, 0.7152, 0.0722);
float LumCalculate(vec3 color, vec3 lume)
{
	return dot(color, lume);
}

vec3 EdgeDetective()
{
	vec3 edgeColor;
	vec3 samplerColor = vec3(0.0);
	float g = 0.0f;

	//	Offset
	float offset = 1.0 / 300.0;
	vec2 offsets[9] = {
		vec2(-offset, offset),	vec2(0, offset),	vec2(offset, offset),
		vec2(-offset, 0),		vec2(0, 0),			vec2(offset, 0),
		vec2(-offset, -offset),	vec2(0, -offset),	vec2(offset, -offset)
	};

	//	Sobel
	int Sx[9] = {
		-1, 0, 1,
		-2,	0, 2,
		-1, 0, 1
	};

	int Sy[9] = {
	   -1, -2, -1,
		0,	0,	0,
		1,	2,	1
	};

	float x = 0.0;
	float y = 0.0;
	for(int i=0; i<9; i++)
	{
		x += LumCalculate(texture(screen_texture, TexCoord + offsets[i]).rgb * Sx[i], lume);
		y += LumCalculate(texture(screen_texture, TexCoord + offsets[i]).rgb * Sy[i], lume);
	}
	g = x*x + y*y;

	if(g > 0.05)
	{
		return vec3(1.0f);
	}
	else
	{
		return vec3(0.0f);
	}
}

void main()
{
	vec3 color = EdgeDetective();
	FragColor = vec4(color, 1.0);
}