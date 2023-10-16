#version 460
layout (triangles_adjacency) in;
layout( triangle_strip, max_vertices = 3 ) out;

in vec3 Normal[];
in vec3 FragPos[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float edgeWidth;
uniform float pctExtend;

flat out int isEdge;
out vec3 NormalG;
out vec3 FragPosG;

void main()
{

}