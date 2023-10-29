#version 430
in vec2 texCoord;
in vec3 fragPos;

uniform sampler2D tex_color;
uniform sampler2D tex_normal;
uniform mat4 model;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

out vec4 FragColor;

mat3 GetTBN(vec3 normal) {
	vec3 randomVec = glm::vec3(0.0, 1.0, 0.0);

	vec3 t = normalize(randomVec - normal * (dot(randomVec, normal)));
	vec3 b = normalize(cross(normal, t));
	mat3 TBN = mat3(t, b, normal);
	return TBN;
}

void main()
{
	vec3 color = textureLod(tex_color, texCoord, 0).rgb;

	vec3 normal = texture(tex_normal, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 nnormal = normalize( normalMatrix*vec3(0,1,0));
	vec3 tangent =normalize( normalMatrix*vec3(1,0,0));
	vec3 bitangent = cross(tangent, nnormal);
	mat3 tbn = mat3(tangent, bitangent, nnormal);

	normal = normalize(tbn * normal);

	vec3 lightVec = normalize(lightPos - fragPos);
	vec3 viewVec = normalize(cameraPos - fragPos);
	vec3 halfVec = normalize(lightVec + viewVec);

	vec3 ambient = vec3(0.2) * color;

	float diffuseRation = max(dot(lightVec, normal), 0.0);
	vec3 diffuse = diffuseRation * lightColor * color;

	float specualrRation = pow(dot(halfVec, normal) , 4096);
	vec3 specular = specualrRation * lightColor * color;

	FragColor = vec4(specular + diffuse + ambient  , 1.0);
}