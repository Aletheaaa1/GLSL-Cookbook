#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

out vec2 texCoord;

void main()
{
	texCoord = aTexture;

	gl_Position = vec4(aPos, 1.0);
}