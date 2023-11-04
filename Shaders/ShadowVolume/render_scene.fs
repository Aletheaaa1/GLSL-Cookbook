#version 460
layout (location = 0) out vec4 ambient;
layout (location = 1) out vec4 diffuse_specuar;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform mat4 view;

void main()
{

	vec3 n = normalize(normal);

	vec3 lightPosV = vec3( view * vec4(lightPos, 1.0));

	vec3 viewDir = normalize(lightPosV - fragPos);
	vec3 lightDir = normalize( -fragPos);
	vec3 halfDir = normalize(viewDir + lightDir);

	float diffuseRation = max(dot(lightDir, n), 0.0);
	vec3 diffuse = diffuseRation * lightColor;
	float specularRation = pow(max(dot(halfDir, n), 0.0), 256);
	vec3 specular = specularRation * lightColor;

	ambient = vec4(vec3(1.0) * 0.3, 1.0);
	diffuse_specuar = vec4(diffuse + specular, 1.0);
}