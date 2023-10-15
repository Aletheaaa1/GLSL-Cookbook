#version 460
layout (points) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform float halfSize;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
	gl_Position = projection * (vec4(-halfSize, -halfSize, 0.0, 0.0) + gl_in[0].gl_Position);
	TexCoord = vec2(0.0, 0.0);
	EmitVertex();

	gl_Position = projection * (vec4(halfSize, -halfSize, 0.0, 0.0) + gl_in[0].gl_Position);
	TexCoord = vec2(1.0, 0.0);
	EmitVertex();

	gl_Position = projection * (vec4(-halfSize, halfSize, 0.0, 0.0) + gl_in[0].gl_Position);
	TexCoord = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = projection * (vec4(halfSize, halfSize, 0.0, 0.0) + gl_in[0].gl_Position);
	TexCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}