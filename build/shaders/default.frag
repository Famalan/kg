#version 330 core

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Color;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform Light lights[4];
uniform bool useTexture;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    // Ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    // Calculate contribution from each light
    for(int i = 0; i < 4; i++) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        
        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color;
        
        // Specular
        float specularStrength = 0.5;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = specularStrength * spec * lights[i].color;
        
        // Attenuation
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        result += (ambient + diffuse + specular) * lights[i].intensity * attenuation;
    }
    
    vec4 finalColor;
    if (useTexture) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        finalColor = vec4(result, 1.0) * texColor;
    } else {
        finalColor = vec4(result * Color, 1.0);
    }
    
    FragColor = finalColor;
} 