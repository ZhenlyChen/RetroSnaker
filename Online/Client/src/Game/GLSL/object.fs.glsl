#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D height;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
	vec3 color;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform vec3 strength;

uniform bool hasColor;
uniform bool hasTexture;

void main() {
    vec3 ambient = strength.x * light.color;
	if (hasTexture) {
		ambient *= texture(material.diffuse, TexCoords).rgb;
	} else {
		ambient *= light.ambient;
	}

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = strength.y * diff * light.color;
	if (hasTexture) {
		diffuse *= texture(material.diffuse, TexCoords).rgb;
	} else  {
		// diffuse *= light.diffuse;
	}

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), light.shininess);
	vec3 specular = strength.z * spec * light.color;
	if (hasTexture) {
		// specular *= texture(material.specular, TexCoords).rgb;
	} else {
		specular *= light.specular;
	}

	vec3 result = ambient + diffuse + specular;

	if (hasTexture) {
		// result *= texture(material.diffuse, TexCoords);
	} else {
		result *= light.diffuse;
	}

	FragColor = vec4(result, 1.0);
}
