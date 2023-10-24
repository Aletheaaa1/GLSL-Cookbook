#version 430
out vec2 texCoord;
void main(void)
{
	vec2 patchTexCoords[] = vec2[](vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1));

	int dx = gl_InstanceID % 64;
	float dy = gl_InstanceID / 64.0;

	texCoord = vec2((dx + patchTexCoords[gl_VertexID].x) / 64.0, (dy + patchTexCoords[gl_VertexID].y) / 64.0);
	gl_Position = vec4(texCoord.x - 0.5, 0.0, texCoord.y - 0.5, 1.0);
}