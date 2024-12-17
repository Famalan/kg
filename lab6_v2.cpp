#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iostream>
#include <map>

// В начале файла добавим определение вершин куба
namespace {
    const std::vector<glm::vec3> cubeVertices = {
        // Передняя грань
        glm::vec3(-0.5f, -0.5f,  0.5f),
        glm::vec3( 0.5f, -0.5f,  0.5f),
        glm::vec3( 0.5f,  0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f,  0.5f),
        // Задняя грань
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3( 0.5f, -0.5f, -0.5f),
        glm::vec3( 0.5f,  0.5f, -0.5f),
        glm::vec3(-0.5f,  0.5f, -0.5f)
    };

    const std::vector<unsigned int> cubeIndices = {
        // Передняя грань
        0, 1, 2, 2, 3, 0,
        // Правая грань
        1, 5, 6, 6, 2, 1,
        // Задняя грань
        5, 4, 7, 7, 6, 5,
        // Левая грань
        4, 0, 3, 3, 7, 4,
        // Верхняя грань
        3, 2, 6, 6, 7, 3,
        // Нижняя грань
        4, 5, 1, 1, 0, 4
    };

    const std::vector<glm::vec3> cubeNormals = {
        glm::vec3( 0.0f,  0.0f,  1.0f), // Передняя
        glm::vec3( 1.0f,  0.0f,  0.0f), // Правая
        glm::vec3( 0.0f,  0.0f, -1.0f), // Задняя
        glm::vec3(-1.0f,  0.0f,  0.0f), // Левая
        glm::vec3( 0.0f,  1.0f,  0.0f), // Верхняя
        glm::vec3( 0.0f, -1.0f,  0.0f)  // Нижняя
    };
}

// Базовые компоненты движка
namespace Engine {
    // Структура для профилирования
    struct ProfilerData {
        float frameTime;
        float fps;
        int objectCount;
        std::string renderMode;
    };

    // Базовый класс для материалов
    class Material {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        Material(const glm::vec3& a = glm::vec3(0.1f), 
                const glm::vec3& d = glm::vec3(0.7f),
                const glm::vec3& s = glm::vec3(1.0f),
                float sh = 32.0f)
            : ambient(a), diffuse(d), specular(s), shininess(sh) {}
    };

    // Базовый класс для источников света
    class Light {
    public:
        enum class Type {
            Directional,
            Point,
            Spot
        };

        Type type;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        float radius;  // Для Point и Spot
        float angle;   // Для Spot

        Light(Type t = Type::Point) : type(t), 
            position(0.0f), direction(0.0f, -1.0f, 0.0f),
            color(1.0f), intensity(1.0f), radius(10.0f), angle(45.0f) {}
    };

    // Базовый класс для игровых объектов
    class GameObject {
    public:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        Material material;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        GameObject() : position(0.0f), rotation(0.0f), scale(1.0f) {}
        virtual void update(float deltaTime) = 0;
        virtual void render() = 0;
        virtual ~GameObject() = default;
    };

    // Класс камеры
    class Camera {
    public:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        float yaw;
        float pitch;
        float fov;
        bool orthographic;

        Camera() : position(0.0f, 0.0f, 3.0f),
                  front(0.0f, 0.0f, -1.0f),
                  up(0.0f, 1.0f, 0.0f),
                  yaw(-90.0f), pitch(0.0f),
                  fov(45.0f), orthographic(false) {}

        glm::mat4 getViewMatrix() const {
            return glm::lookAt(position, position + front, up);
        }

        glm::mat4 getProjectionMatrix(float aspectRatio) const {
            if (orthographic) {
                float scale = 10.0f;
                return glm::ortho(-scale * aspectRatio, scale * aspectRatio,
                                -scale, scale, 0.1f, 100.0f);
            } else {
                return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
            }
        }
    };

    class Shader {
    private:
        GLuint program;
        std::map<std::string, GLint> uniformLocations;

        GLint getUniformLocation(const std::string& name) {
            if (uniformLocations.find(name) == uniformLocations.end()) {
                uniformLocations[name] = glGetUniformLocation(program, name.c_str());
            }
            return uniformLocations[name];
        }

    public:
        Shader(const char* vertexPath, const char* fragmentPath) {
            std::string vertexCode;
            std::string fragmentCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;

            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;
                
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();
                
                vShaderFile.close();
                fShaderFile.close();
                
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
            }
            catch(std::ifstream::failure& e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
            }

            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();

            GLuint vertex, fragment;
            
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            checkCompileErrors(vertex, "VERTEX");
            
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            checkCompileErrors(fragment, "FRAGMENT");
            
            program = glCreateProgram();
            glAttachShader(program, vertex);
            glAttachShader(program, fragment);
            glLinkProgram(program);
            checkCompileErrors(program, "PROGRAM");
            
            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }

        ~Shader() {
            glDeleteProgram(program);
        }

        void use() {
            glUseProgram(program);
        }

        void setBool(const std::string& name, bool value) const {
            glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
        }
        
        void setInt(const std::string& name, int value) const {
            glUniform1i(glGetUniformLocation(program, name.c_str()), value);
        }
        
        void setFloat(const std::string& name, float value) const {
            glUniform1f(glGetUniformLocation(program, name.c_str()), value);
        }
        
        void setVec3(const std::string& name, const glm::vec3& value) const {
            glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
        }
        
        void setMat4(const std::string& name, const glm::mat4& mat) const {
            glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        void checkCompileErrors(GLuint shader, std::string type) {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM") {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
                             << infoLog << std::endl;
                }
            } else {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
                             << infoLog << std::endl;
                }
            }
        }
    };

    // Менеджер сцены
    class Scene {
    private:
        std::vector<std::unique_ptr<GameObject>> objects;
        std::vector<Light> lights;
        Camera camera;
        ProfilerData profiler;
        bool useRayTracing;
        std::unique_ptr<Shader> shader;  // Добавляем шейдер
        float lastX = 400.0f;           // Для управления камерой
        float lastY = 300.0f;
        bool firstMouse = true;

    public:
        Scene() : useRayTracing(false) {
            profiler.frameTime = 0.0f;
            profiler.fps = 0.0f;
            profiler.objectCount = 0;
            profiler.renderMode = "Rasterization";

            // Инициализация шейдера
            shader = std::make_unique<Shader>("shaders/vertex.glsl", "shaders/fragment.glsl");
            
            // Настройка OpenGL
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }

        void processInput(sf::Window& window, float deltaTime) {
            float cameraSpeed = 2.5f * deltaTime;
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                camera.position += cameraSpeed * camera.front;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                camera.position -= cameraSpeed * camera.front;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                camera.position += camera.up * cameraSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                camera.position -= camera.up * cameraSpeed;
            
            // Переключение проекции
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                camera.orthographic = !camera.orthographic;
        }

        void processMouseMovement(float xpos, float ypos) {
            if (firstMouse) {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            camera.yaw += xoffset;
            camera.pitch += yoffset;

            if (camera.pitch > 89.0f)
                camera.pitch = 89.0f;
            if (camera.pitch < -89.0f)
                camera.pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
            front.y = sin(glm::radians(camera.pitch));
            front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
            camera.front = glm::normalize(front);
        }

        void processMouseScroll(float yoffset) {
            camera.fov -= yoffset;
            if (camera.fov < 1.0f)
                camera.fov = 1.0f;
            if (camera.fov > 90.0f)
                camera.fov = 90.0f;
        }

        void addObject(std::unique_ptr<GameObject> obj) {
            objects.push_back(std::move(obj));
            profiler.objectCount = objects.size();
        }

        void addLight(const Light& light) {
            lights.push_back(light);
        }

        void update(float deltaTime) {
            for (auto& obj : objects) {
                obj->update(deltaTime);
            }
        }

        void render() {
            auto startTime = std::chrono::high_resolution_clock::now();

            if (useRayTracing) {
                renderRayTracing();
            } else {
                renderRasterization();
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(endTime - startTime).count();
            updateProfiler(frameTime);
        }

    private:
        void renderRasterization() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader->use();

            // Установка матриц камеры
            float aspectRatio = 800.0f / 600.0f; // TODO: получать реальные размеры окна
            glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
            glm::mat4 view = camera.getViewMatrix();
            
            shader->setMat4("projection", projection);
            shader->setMat4("view", view);
            shader->setVec3("viewPos", camera.position);

            // Установка информации об освещении
            shader->setInt("numLights", lights.size());
            for (size_t i = 0; i < lights.size(); i++) {
                std::string prefix = "lights[" + std::string(1, '0' + i) + "].";
                shader->setInt(prefix + "type", static_cast<int>(lights[i].type));
                shader->setVec3(prefix + "position", lights[i].position);
                shader->setVec3(prefix + "direction", lights[i].direction);
                shader->setVec3(prefix + "color", lights[i].color);
                shader->setFloat(prefix + "intensity", lights[i].intensity);
                shader->setFloat(prefix + "radius", lights[i].radius);
                shader->setFloat(prefix + "angle", lights[i].angle);
            }

            // Отрисовка объектов
            for (const auto& obj : objects) {
                // Установка материала
                shader->setVec3("material.ambient", obj->material.ambient);
                shader->setVec3("material.diffuse", obj->material.diffuse);
                shader->setVec3("material.specular", obj->material.specular);
                shader->setFloat("material.shininess", obj->material.shininess);
                
                obj->render();
            }
        }

        void renderRayTracing() {
            // Реализация трассировки лучей
            // TODO: Добавить код трассировки лучей
        }

        void updateProfiler(float frameTime) {
            profiler.frameTime = frameTime;
            profiler.fps = 1.0f / frameTime;
            profiler.renderMode = useRayTracing ? "Ray Tracing" : "Rasterization";
        }
    };
}

// Пример конкретного игрового объекта
class Cube : public Engine::GameObject {
private:
    GLuint vao, vbo, ebo;

public:
    Cube() {
        // Копируем вершины и индексы
        vertices = cubeVertices;
        indices = cubeIndices;
        
        // Инициализация OpenGL буферов
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        // Вершины
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), 
                    vertices.data(), GL_STATIC_DRAW);

        // Индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                    indices.data(), GL_STATIC_DRAW);

        // Настройка атрибутов вершин
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(0);
    }

    void update(float deltaTime) override {
        rotation.y += deltaTime * 45.0f;
    }

    void render() override {
        glBindVertexArray(vao);
        
        // Создаем матрицу модели
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);

        // TODO: Установить uniform переменные для шейдера
        
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Cube() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }
};

int main() {
    // Создание окна с поддержкой OpenGL
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Game Engine",
                           sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Создание сцены
    Engine::Scene scene;

    // Добавление объектов и источников света
    scene.addObject(std::unique_ptr<Cube>(new Cube()));
    
    Engine::Light light(Engine::Light::Type::Point);
    light.position = glm::vec3(5.0f, 5.0f, 5.0f);
    scene.addLight(light);

    // Захват курсора мыши
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    // Основной цикл
    sf::Clock clock;
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseMoved) {
                scene.processMouseMovement(event.mouseMove.x, event.mouseMove.y);
            }
            else if (event.type == sf::Event::MouseWheelScrolled) {
                scene.processMouseScroll(event.mouseWheelScroll.delta);
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        // Обработка ввода
        scene.processInput(window, deltaTime);

        // Обновление и рендеринг сцены
        scene.update(deltaTime);
        scene.render();

        window.display();
    }

    return 0;
} 