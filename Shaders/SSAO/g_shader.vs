#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(view * model)));

	Normal = normalMatrix * aNormal;
	FragPos = (view * model * vec4(aPos, 1.0)).xyz;
	TexCoord = aTexture;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}