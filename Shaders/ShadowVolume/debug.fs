#version 460
out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D scene;

void main()
{
	fragColor = vec4(texture(scene, texCoord).rgb , 1.0);
}