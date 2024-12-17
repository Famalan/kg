#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <sstream>

// Структуры для описания объектов сцены
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    
    Ray(const glm::vec3& o, const glm::vec3& d) 
        : origin(o), direction(glm::normalize(d)) {}
};

struct Material {
    glm::vec3 color;
    float reflectivity;
    float specular;
    
    Material(const glm::vec3& c = glm::vec3(1.0f), float r = 0.0f, float s = 0.0f)
        : color(c), reflectivity(r), specular(s) {}
};

struct Light {
    glm::vec3 position;
    float radius;  // Размер источника света для мягких теней
    float intensity;
    
    Light(const glm::vec3& pos, float r = 0.5f, float i = 1.0f)
        : position(pos), radius(r), intensity(i) {}
};

class Object {
public:
    Material material;
    virtual bool intersect(const Ray& ray, float& t) const = 0;
    virtual glm::vec3 getNormal(const glm::vec3& point) const = 0;
    virtual ~Object() {}
};

class Sphere : public Object {
    glm::vec3 center;
    float radius;
public:
    Sphere(const glm::vec3& c, float r, const Material& m) 
        : center(c), radius(r) {
        material = m;
    }
    
    bool intersect(const Ray& ray, float& t) const override {
        glm::vec3 oc = ray.origin - center;
        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        
        if (discriminant < 0) return false;
        
        float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
        float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
        
        if (t0 > t1) std::swap(t0, t1);
        
        if (t0 < 0) {
            t0 = t1;
            if (t0 < 0) return false;
        }
        
        t = t0;
        return true;
    }
    
    glm::vec3 getNormal(const glm::vec3& point) const override {
        return glm::normalize(point - center);
    }
};

class Plane : public Object {
    glm::vec3 normal;
    float distance;
public:
    Plane(const glm::vec3& n, float d, const Material& m) 
        : normal(glm::normalize(n)), distance(d) {
        material = m;
    }
    
    bool intersect(const Ray& ray, float& t) const override {
        float denom = glm::dot(normal, ray.direction);
        if (std::abs(denom) > 1e-6) {
            t = -(glm::dot(ray.origin, normal) + distance) / denom;
            return t >= 0;
        }
        return false;
    }
    
    glm::vec3 getNormal(const glm::vec3& point) const override {
        return normal;
    }
};

class Scene {
    std::vector<Object*> objects;
    std::vector<Light> lights;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

public:
    Scene() : dist(-1.0f, 1.0f) {
        std::random_device rd;
        rng = std::mt19937(rd());
    }
    
    void addObject(Object* obj) {
        objects.push_back(obj);
    }
    
    void addLight(const Light& light) {
        lights.push_back(light);
    }
    
    glm::vec3 randomPointOnLight(const Light& light) {
        float r = light.radius * sqrt(dist(rng) + 1) / 2.0f;
        float theta = dist(rng) * M_PI;
        float x = r * cos(theta);
        float y = r * sin(theta);
        return light.position + glm::vec3(x, 0, y);
    }
    
    bool trace(const Ray& ray, float& t, Object const** hitObject) const {
        t = INFINITY;
        *hitObject = nullptr;
        
        for (const auto& object : objects) {
            float tTemp;
            if (object->intersect(ray, tTemp) && tTemp < t) {
                t = tTemp;
                *hitObject = object;
            }
        }
        
        return *hitObject != nullptr;
    }
    
    float calculateShadow(const glm::vec3& point, const Light& light, int samples = 16) {
        float shadow = 0.0f;
        
        for (int i = 0; i < samples; ++i) {
            glm::vec3 lightPos = randomPointOnLight(light);
            glm::vec3 lightDir = glm::normalize(lightPos - point);
            Ray shadowRay(point + lightDir * 0.001f, lightDir);
            
            float t;
            Object const* hitObject;
            float dist = glm::length(lightPos - point);
            
            if (trace(shadowRay, t, &hitObject) && t < dist) {
                shadow += 1.0f;
            }
        }
        
        return 1.0f - shadow / float(samples);
    }
    
    glm::vec3 calculateColor(const Ray& ray, int depth = 0) {
        if (depth > 3) return glm::vec3(0.0f);
        
        float t;
        Object const* hitObject;
        if (!trace(ray, t, &hitObject)) {
            return glm::vec3(0.1f); // Фоновое освещение
        }
        
        glm::vec3 hitPoint = ray.origin + ray.direction * t;
        glm::vec3 normal = hitObject->getNormal(hitPoint);
        glm::vec3 color(0.0f);
        
        // Освещение и мягкие тени
        for (const auto& light : lights) {
            float shadowFactor = calculateShadow(hitPoint, light);
            
            glm::vec3 lightDir = glm::normalize(light.position - hitPoint);
            float diff = std::max(glm::dot(normal, lightDir), 0.0f);
            
            glm::vec3 reflectDir = glm::reflect(-lightDir, normal);
            float spec = std::pow(std::max(glm::dot(-ray.direction, reflectDir), 0.0f), 32.0f);
            
            color += hitObject->material.color * light.intensity * 
                    (diff + hitObject->material.specular * spec) * shadowFactor;
        }
        
        // Отражения
        if (hitObject->material.reflectivity > 0.0f) {
            glm::vec3 reflectDir = glm::reflect(ray.direction, normal);
            Ray reflectRay(hitPoint + reflectDir * 0.001f, reflectDir);
            color += hitObject->material.reflectivity * 
                    calculateColor(reflectRay, depth + 1);
        }
        
        return color;
    }
    
    void updateLightRadius(float radius) {
        if (!lights.empty()) {
            lights[0].radius = radius;
        }
    }
    
    float getLightRadius() const {
        return lights.empty() ? 0.0f : lights[0].radius;
    }
    
    ~Scene() {
        for (auto obj : objects) {
            delete obj;
        }
    }
};

int main() {
    const int width = 800;
    const int height = 600;
    
    sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracer");
    sf::Image image;
    image.create(width, height);
    sf::Texture texture;
    sf::Sprite sprite;
    
    Scene scene;
    
    // Добавляем объекты в сцену
    Material sphereMaterial(glm::vec3(1.0f, 0.2f, 0.2f), 0.5f, 0.8f);
    Material planeMaterial(glm::vec3(0.5f, 0.5f, 0.5f), 0.1f, 0.1f);
    
    scene.addObject(new Sphere(glm::vec3(0.0f, 0.0f, -5.0f), 1.0f, sphereMaterial));
    scene.addObject(new Plane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, planeMaterial));
    
    // Добавляем источник света
    Light light(glm::vec3(5.0f, 5.0f, 5.0f), 1.0f, 1.0f);
    scene.addLight(light);
    
    // Параметры камеры
    glm::vec3 cameraPos(0.0f, 0.0f, 0.0f);
    float aspectRatio = float(width) / float(height);
    
    bool needUpdate = true;
    float lightRadius = 1.0f;
    
    // Добавим шрифт для отображения информации
    sf::Font font;
    bool fontLoaded = false;
    // Пробуем разные пути к шрифтам
    const std::vector<std::string> fontPaths = {
        "/System/Library/Fonts/Helvetica.ttf",  // macOS
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
        "C:\\Windows\\Fonts\\arial.ttf",  // Windows
        "arial.ttf",  // Локальная директория
        "/usr/share/fonts/TTF/DejaVuSans.ttf"  // Альтернативный путь для Linux
    };
    
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    
    if (!fontLoaded) {
        std::cout << "Warning: Could not load any font, continuing without text overlay" << std::endl;
    }
    
    sf::Text infoText;
    if (fontLoaded) {
        infoText.setFont(font);
        infoText.setCharacterSize(14);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(10, 10);
    }
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    lightRadius = std::min(lightRadius + 0.1f, 2.0f);
                    needUpdate = true;
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    lightRadius = std::max(lightRadius - 0.1f, 0.1f);
                    needUpdate = true;
                }
                // Добавим быстрое изменение размера
                else if (event.key.code == sf::Keyboard::PageUp) {
                    lightRadius = std::min(lightRadius + 0.5f, 2.0f);
                    needUpdate = true;
                }
                else if (event.key.code == sf::Keyboard::PageDown) {
                    lightRadius = std::max(lightRadius - 0.5f, 0.1f);
                    needUpdate = true;
                }
            }
        }
        
        if (needUpdate) {
            scene.updateLightRadius(lightRadius);
            
            // Рендеринг сцены
            #pragma omp parallel for collapse(2)
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    float u = (2.0f * x / width - 1.0f) * aspectRatio;
                    float v = 1.0f - 2.0f * y / height;
                    
                    Ray ray(cameraPos, glm::vec3(u, v, -1.0f));
                    glm::vec3 color = scene.calculateColor(ray);
                    
                    // Гамма-коррекция и преобразование в цвет SFML
                    color = glm::pow(color, glm::vec3(1.0f/2.2f));
                    sf::Color pixelColor(
                        uint8_t(std::min(color.r * 255.0f, 255.0f)),
                        uint8_t(std::min(color.g * 255.0f, 255.0f)),
                        uint8_t(std::min(color.b * 255.0f, 255.0f))
                    );
                    image.setPixel(x, y, pixelColor);
                }
            }
            
            texture.loadFromImage(image);
            sprite.setTexture(texture);
            needUpdate = false;
        }
        
        window.clear();
        window.draw(sprite);
        if (fontLoaded) {  // Отображаем текст только если шрифт загружен
            // Обновляем текст с информацией
            std::ostringstream ss;
            ss << "Light Radius: " << lightRadius << "\n"
               << "Controls:\n"
               << "Up/Down: Fine adjust radius\n"
               << "PgUp/PgDown: Coarse adjust radius\n"
               << "Current shadow softness: " 
               << (lightRadius < 0.5f ? "Hard" : 
                   lightRadius < 1.0f ? "Medium" : "Soft");
            infoText.setString(ss.str());
            window.draw(infoText);
        }
        window.display();
    }
    
    return 0;
}
