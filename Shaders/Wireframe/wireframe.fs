#version 460
out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform float lineWidth;
uniform vec3 lineColor;

in vec3 NormalG;
in vec3 EdgeDistance;
in vec3 FragPosG;

vec3 PhongMode()
{
	vec3 lightDir = normalize(lightPos - FragPosG);
	vec3 viewDir = normalize(cameraPos - FragPosG);
	vec3 halfDir = normalize(lightDir + viewDir);

	vec3 ambient = vec3(0.3);

	float diffuseRation = max(dot(lightDir, NormalG), 0.0);
	vec3 diffuse = diffuseRation * lightColor;

	float specularRation = pow(max(dot(halfDir, NormalG), 0), 32);
	vec3 specular = specularRation * lightColor;

	return ambient + diffuse  + specular ;
}

void main()
{
	vec3 color = PhongMode();

	//	smallest distance
	float smallestDistance = min(EdgeDistance.x, EdgeDistance.y);
	smallestDistance = min(smallestDistance, EdgeDistance.z);

    float mixVal = smoothstep(lineWidth - 0.2, lineWidth + 0.2, smallestDistance);
	FragColor = vec4(mix( lineColor, color, mixVal ), 1.0);
}