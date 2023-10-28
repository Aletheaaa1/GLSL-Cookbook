#version 460
in vec2 texCoord;

uniform sampler2D shadow_texture;

out vec4 fragColor;

void main()
{
	float depth = texture(shadow_texture, texCoord).r;
	fragColor = vec4(vec3(depth), 1.0);
}