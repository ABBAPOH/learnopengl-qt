#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 viewPos;

uniform Light light;
uniform Material material;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(light.position - FragPos);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                               light.quadratic * (distance * distance));

    float diff = max(dot(normal, lightDirection), 0.0);

    // ambient & diffuse & specular color
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords)) * attenuation;
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords)) * attenuation;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords)) * attenuation;

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
}
