#version 330 core

struct Camera {
    vec3 position;
    vec3 target;
    vec3 up;
};

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    float reflectivity;
};

in vec2 TexCoords;
out vec4 FragColor;

uniform Camera camera;
uniform Light lights[4];
uniform Sphere spheres[16];
uniform int sphereCount;
uniform int maxBounces;

Ray generateRay() {
    vec3 forward = normalize(camera.target - camera.position);
    vec3 right = normalize(cross(forward, camera.up));
    vec3 up = cross(right, forward);
    
    float fov = 60.0;
    float aspectRatio = 16.0 / 9.0;
    float scale = tan(radians(fov * 0.5));
    
    vec2 xy = TexCoords * 2.0 - 1.0;
    xy.x *= aspectRatio;
    
    vec3 direction = normalize(forward + right * xy.x * scale + up * xy.y * scale);
    
    return Ray(camera.position, direction);
}

bool intersectSphere(Ray ray, Sphere sphere, out float t) {
    vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4.0 * a * c;
    
    if (discriminant < 0.0) return false;
    
    float t0 = (-b - sqrt(discriminant)) / (2.0 * a);
    float t1 = (-b + sqrt(discriminant)) / (2.0 * a);
    
    t = (t0 < 0.0) ? t1 : t0;
    return t >= 0.0;
}

vec3 traceRay(Ray ray, int bounce) {
    if (bounce >= maxBounces) return vec3(0.0);
    
    float closest_t = 1e20;
    int closest_sphere = -1;
    
    // Find closest intersection
    for (int i = 0; i < sphereCount; i++) {
        float t;
        if (intersectSphere(ray, spheres[i], t)) {
            if (t < closest_t) {
                closest_t = t;
                closest_sphere = i;
            }
        }
    }
    
    if (closest_sphere == -1) return vec3(0.0);
    
    Sphere sphere = spheres[closest_sphere];
    vec3 hitPoint = ray.origin + ray.direction * closest_t;
    vec3 normal = normalize(hitPoint - sphere.center);
    
    vec3 color = vec3(0.0);
    
    // Direct lighting
    for (int i = 0; i < 4; i++) {
        vec3 lightDir = normalize(lights[i].position - hitPoint);
        float diffuse = max(dot(normal, lightDir), 0.0);
        
        // Shadow ray
        Ray shadowRay = Ray(hitPoint + normal * 0.001, lightDir);
        bool inShadow = false;
        
        for (int j = 0; j < sphereCount; j++) {
            float t;
            if (j != closest_sphere && intersectSphere(shadowRay, spheres[j], t)) {
                inShadow = true;
                break;
            }
        }
        
        if (!inShadow) {
            color += sphere.color * lights[i].color * lights[i].intensity * diffuse;
        }
    }
    
    // Reflection
    if (sphere.reflectivity > 0.0 && bounce < maxBounces) {
        vec3 reflectDir = reflect(ray.direction, normal);
        Ray reflectRay = Ray(hitPoint + normal * 0.001, reflectDir);
        color += sphere.reflectivity * traceRay(reflectRay, bounce + 1);
    }
    
    return color;
}

void main() {
    Ray ray = generateRay();
    vec3 color = traceRay(ray, 0);
    FragColor = vec4(color, 1.0);
} 