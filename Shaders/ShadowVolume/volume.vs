#version 410
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 normal;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	normal = normalMatrix * aNormal;
	fragPos = vec3(view * model * vec4(aPos, 1.0));

	gl_Position =   view * model * vec4(aPos, 1.0);
}