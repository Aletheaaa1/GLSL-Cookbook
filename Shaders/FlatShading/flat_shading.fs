#version 460
flat in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;
in vec3 innerNormal;

void main()
{
	if(gl_FrontFacing)
		FragColor = vec4(Normal, 1.0);
	else
		FragColor = vec4(FragPos, 1.0);
}