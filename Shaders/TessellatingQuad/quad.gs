#version 400

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

 out vec3 EdgeDistance;

void main()
{
    // Transform each vertex into viewport space
    vec3 p0 = vec3((gl_in[0].gl_Position));
    vec3 p1 = vec3((gl_in[1].gl_Position));
    vec3 p2 = vec3((gl_in[2].gl_Position));

    gl_Position = vec4(p0, 1.0);
    EmitVertex();

    gl_Position = vec4(p1, 1.0);
    EmitVertex();

    gl_Position = vec4(p2, 1.0);
    EmitVertex();

    EndPrimitive();
}