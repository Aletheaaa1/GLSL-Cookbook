#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
	mat3 NormalMatrix = transpose(inverse(mat3(view * model)));
	Normal =normalize( NormalMatrix * aNormal);
	FragPos = (view * model * vec4(aPos, 1.0)).xyz;

	gl_Position = model * vec4(aPos, 1.0);
}