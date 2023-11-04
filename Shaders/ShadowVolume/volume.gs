#version 410
layout( triangles_adjacency ) in;
layout( triangle_strip, max_vertices = 18 ) out;

in vec3 fragPos[];
in vec3 normal[];

uniform vec3 lightPos;
uniform mat4 view;
uniform mat4 projection;

bool FaceLight(vec3 a, vec3 b, vec3 c)
{
	vec3 lightPosV = vec3(view * vec4(lightPos, 1.0));

	vec3 n = cross(b - a, c - a);
	vec3 da = lightPosV - a;
	vec3 db = lightPosV - b;
	vec3 dc = lightPosV - c;

	return dot(n, da) > 0 || dot(n, db) > 0 || dot(n, dc) > 0;
}

void EmitEdge(vec3 a, vec3 b)
{
	vec3 lightPosV = vec3(view * vec4(lightPos, 1.0));

	gl_Position = projection * vec4(a, 1.0);
	EmitVertex();

	gl_Position = projection * vec4(a - lightPosV, 0.0);
	EmitVertex();

	gl_Position = projection * vec4(b, 1.0);
	EmitVertex();

	gl_Position = projection * vec4(b - lightPosV, 0.0);
	EmitVertex();

	EndPrimitive();
}

void main()
{
//	if(FaceLight(fragPos[0], fragPos[2], fragPos[4]))
//	{
//		if(!FaceLight(fragPos[0], fragPos[1], fragPos[2]))
//		{
//			EmitEdge(fragPos[0], fragPos[2]);
//		}
//
//		if(!FaceLight(fragPos[2], fragPos[3], fragPos[4]))
//		{
//			EmitEdge(fragPos[2], fragPos[4]);
//		}
//
//		if(!FaceLight(fragPos[4], fragPos[5], fragPos[0]))
//		{
//			EmitEdge(fragPos[4], fragPos[0]);
//		}
//	}
	if(FaceLight(gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz, gl_in[4].gl_Position.xyz))
	{
		if(!FaceLight(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz))
		{
			EmitEdge(gl_in[0].gl_Position.xyz,  gl_in[2].gl_Position.xyz);
		}

		if(!FaceLight( gl_in[2].gl_Position.xyz,  gl_in[3].gl_Position.xyz,  gl_in[4].gl_Position.xyz))
		{
			EmitEdge(gl_in[2].gl_Position.xyz, gl_in[4].gl_Position.xyz);
		}

		if(!FaceLight(gl_in[4].gl_Position.xyz, gl_in[5].gl_Position.xyz, gl_in[0].gl_Position.xyz))
		{
			EmitEdge(gl_in[4].gl_Position.xyz,  gl_in[0].gl_Position.xyz);
		}
	}
}