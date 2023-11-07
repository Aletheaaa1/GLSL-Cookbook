#version 460
in vec2 texCoord;

uniform sampler2D noise_texture;

out vec4 fragColor;

uniform vec4 skyColor = vec4(0.3, 0.3, 0.9, 1.0);
uniform vec4 cloudColor = vec4(1.0);

#define PI 3.1415926

void main()
{
	vec4 noise = texture(noise_texture, texCoord * 0.25) ;

	if(noise.a < 0.1)
	{
		noise.a = 0;
	}
	float t = (cos(noise.a * PI) + 1.0) / 2.0;
	vec4 color = mix(skyColor, cloudColor, t);

	fragColor = vec4(color.rgb, 1.0);
}