#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool is_stencil;

out vec3 Normal;
out vec3 FragPos;
void main()
{
	mat3 NormalMat = transpose(inverse(mat3(model)));
	Normal = normalize(NormalMat * aNormal);

	FragPos = (model * vec4(aPos, 1.0)).xyz;

	if(is_stencil == true)
	{
		gl_Position =  projection * view * model * vec4((aPos + 0.2 * aNormal), 1.0);
	}
	else
	{
		gl_Position = projection * view * model * vec4(aPos, 1.0);
	}
}