#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    int type;  // 0 = directional, 1 = point, 2 = spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float radius;
    float angle;
};

uniform Material material;
uniform Light lights[10];
uniform int numLights;
uniform vec3 viewPos;

vec3 calculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir;
    float attenuation = 1.0;
    
    if(light.type == 0) {  // Directional light
        lightDir = normalize(-light.direction);
    } else {  // Point or spot light
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        if(light.type == 2) {  // Spot light
            float theta = dot(lightDir, normalize(-light.direction));
            float epsilon = cos(radians(light.angle));
            attenuation *= clamp((theta - epsilon) / (0.1), 0.0, 1.0);
        }
    }
    
    // Ambient
    vec3 ambient = material.ambient * light.color;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.color;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.color;
    
    return (ambient + diffuse + specular) * light.intensity * attenuation;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);
    
    for(int i = 0; i < numLights; i++) {
        result += calculateLight(lights[i], norm, FragPos, viewDir);
    }
    
    FragColor = vec4(result, 1.0);
} 