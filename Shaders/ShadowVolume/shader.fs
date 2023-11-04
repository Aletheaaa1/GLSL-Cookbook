#version 460
out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

uniform sampler2D scene;

void main()
{

	fragColor = vec4(texture(scene, texCoord));
}