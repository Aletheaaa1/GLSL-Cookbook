#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main()
{
	mat3 normalMatrix =  transpose(inverse(mat3(view * model)));
	fragPos = vec3(view * model * vec4(aPos, 1.0));
	texCoord = aTexture;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}