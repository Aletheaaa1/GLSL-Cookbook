#version 460
in vec2 texCoord;

uniform sampler2D noise_texture;

out vec4 fragColor;

void main()
{
	fragColor = vec4(vec3(texture(noise_texture, texCoord).a), 1.0);
}