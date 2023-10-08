#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out vec3 Normal;
out vec3 innerNormal;
out vec3 FragPos;
void main()
{
	mat3 NormalMat = transpose(inverse(mat3(model)));
	Normal = normalize(NormalMat * aNormal);
	innerNormal = -Normal;

	FragPos = NormalMat * aPos;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}