#version 400 core
out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 uv;

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform vec3 diffuseColor;

uniform vec3 lightDirection;

uniform float c1;
uniform float c2;
uniform float c3;

uniform float shininess;
uniform vec3 camPos;
uniform vec3 specularColor;

void main()
{
    vec3 objectColor = vec3(0.5f, 0.5f, 0.5f);
    vec3 normal = normalize(fNor);
    vec3 toLight = lightDirection - fPos;
    float distance = length(toLight);
    vec3 normalizedToLight = normalize(toLight);

    vec3 reflection = reflect(-normalizedToLight, normal);
    vec3 toObject = normalize(fPos - camPos);
    float specular = pow(max(dot(reflection, toObject), 0), shininess);

    float diffuse = max(dot(normal, normalizedToLight), 0.0);
    float attenuation = 1.0 / (c1 + c2 * distance + c3 * pow(distance, 2));

    FragColor = vec4(objectColor * ((diffuse * diffuseColor * attenuation) + (ambientColor * ambientIntensity) + (specular * specularColor * attenuation)), 1);
    //FragColor = vec4(objectColor * (diffuse * diffuseColor * attenuation), 1.0);
    //FragColor = vec4(diffuseColor, 1.0);
}