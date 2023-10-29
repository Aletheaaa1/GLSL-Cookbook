#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool normalReverse;
uniform mat4 lightViwe;
uniform mat4 lightProjection;

out vec4 lightSpaceFragPos;
out vec3 normal;
out vec2 texCoord;
out vec3 fragPos;
void main()
{
	if(normalReverse == true)
	{
		normal = - aNormal;
	}
	else
	{
		normal = aNormal;
	}

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	normal = normalize(normalMatrix * normal);

	fragPos = (model * vec4(aPos, 1.0)).xyz;

	lightSpaceFragPos = lightProjection * lightViwe * vec4(fragPos, 1.0);

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}