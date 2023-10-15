#version 460
out vec4 FragColor;

uniform sampler2D scene;

in vec2 TexCoord;

void main()
{
	FragColor = texture(scene, TexCoord);
}