#version 460
layout (triangles) in;
layout( triangle_strip, max_vertices = 3 ) out;

in vec3 Normal[];
in vec3 FragPos[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 NormalG;
out vec3 FragPosG;
out vec3 EdgeDistance;

void main()
{
	vec3 p0 = (projection * view * gl_in[0].gl_Position).xyz;
	vec3 p1 = (projection * view * gl_in[1].gl_Position).xyz;
	vec3 p2 = (projection * view * gl_in[2].gl_Position).xyz;

	float a = length(p1 - p2);
	float b = length(p2 - p0);
	float c = length(p1 - p0);

	float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
    float beta = acos( (a*a + c*c - b*b) / (2.0*a*c) );
	float ha = abs( c * sin( beta ) );
    float hb = abs( c * sin( alpha ) );
    float hc = abs( b * sin( alpha ) );
	ha = clamp(0.0, 1.0 ,ha);
	hb = clamp(0.0, 1.0 ,hb);
	hc = clamp(0.0, 1.0 ,hc);

	gl_Position = projection * view * gl_in[0].gl_Position;
	NormalG = Normal[0];
	FragPosG = FragPos[0];
	EdgeDistance = vec3(ha, 0.0, 0.0);
	EmitVertex();

	gl_Position = projection * view * gl_in[1].gl_Position;
	NormalG = Normal[1];
	FragPosG = FragPos[1];
	EdgeDistance = vec3(0.0, hb, 0.0);
	EmitVertex();

	gl_Position = projection * view * gl_in[2].gl_Position;
	NormalG = Normal[2];
	FragPosG = FragPos[2];
	EdgeDistance = vec3(0.0, 0.0, hc);
	EmitVertex();

	EndPrimitive();
}