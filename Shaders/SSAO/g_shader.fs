#version 460
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

void main()
{
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gColor = vec4(vec3(1.0), 1.0);

}