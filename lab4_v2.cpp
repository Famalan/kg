#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

// Структура для хранения состояния камеры
struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    
    Camera() : 
        position(glm::vec3(0.0f, 0.0f, 5.0f)),
        front(glm::vec3(0.0f, 0.0f, -1.0f)),
        up(glm::vec3(0.0f, 1.0f, 0.0f)),
        yaw(-90.0f),
        pitch(0.0f),
        speed(2.5f),
        sensitivity(0.05f) {}
};

// Структура для хранения параметров освещения
struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float speed;

    Light() :
        position(glm::vec3(2.0f, 2.0f, 2.0f)),
        ambient(glm::vec3(0.2f, 0.2f, 0.2f)),
        diffuse(glm::vec3(0.5f, 0.5f, 0.5f)),
        specular(glm::vec3(1.0f, 1.0f, 1.0f)),
        speed(2.0f) {}
};

// Глобальные переменные
Camera camera;
Light light;
bool firstMouse = true;
float lastX = 400.0f;
float lastY = 300.0f;
float fov = 45.0f;

void drawSphere(float radius, int stacks, int sectors) {
    std::vector<float> vertices;
    std::vector<float> normals;

    // Генерация вершин и нормалей для сферы
    for(int i = 0; i <= stacks; ++i) {
        float phi = M_PI * float(i) / float(stacks);
        for(int j = 0; j <= sectors; ++j) {
            float theta = 2.0f * M_PI * float(j) / float(sectors);
            
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Нормали для сферы совпадают с нормализованными координатами вершин
            float length = sqrt(x*x + y*y + z*z);
            normals.push_back(x/length);
            normals.push_back(y/length);
            normals.push_back(z/length);
        }
    }

    // Отрисовка сферы
    glBegin(GL_QUADS);
    for(int i = 0; i < stacks; ++i) {
        for(int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;
            
            for(int k = 0; k < 4; ++k) {
                int curr = k < 2 ? first : second;
                curr += k % 2;
                
                // Установка нормали для текущей вершины
                glNormal3f(normals[curr*3], normals[curr*3+1], normals[curr*3+2]);
                // Установка вершины
                glVertex3f(vertices[curr*3], vertices[curr*3+1], vertices[curr*3+2]);
            }
        }
    }
    glEnd();
}

void processInput(sf::Window& window, float deltaTime) {
    // Управление камерой
    float velocity = camera.speed * deltaTime;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        camera.position += velocity * camera.front;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        camera.position -= velocity * camera.front;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        camera.position += camera.up * velocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        camera.position -= camera.up * velocity;

    // Управление источником света
    float lightVelocity = light.speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        light.position.z -= lightVelocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        light.position.z += lightVelocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
        light.position.x -= lightVelocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        light.position.x += lightVelocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
        light.position.y += lightVelocity;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        light.position.y -= lightVelocity;
}

void processMouseMovement(float xpos, float ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= camera.sensitivity;
    yoffset *= camera.sensitivity;

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

int main() {
    // Создание окна SFML с поддержкой OpenGL
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    sf::Window window(sf::VideoMode(800, 600), "Point Light Demo", 
                     sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Настройка OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
    glViewport(0, 0, 800, 600);

    // Материал для сферы
    GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat material_shininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    sf::Clock clock;
    
    while (window.isOpen()) {
        float currentFrame = clock.getElapsedTime().asSeconds();
        static float lastFrame = 0.0f;
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::MouseMoved) {
                processMouseMovement(event.mouseMove.x, event.mouseMove.y);
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Настройка проекции
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspectRatio = static_cast<float>(window.getSize().x) / window.getSize().y;
        glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
        glLoadMatrixf(glm::value_ptr(projection));

        // Настройка вида
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glm::mat4 view = glm::lookAt(camera.position, 
                                    camera.position + camera.front,
                                    camera.up);
        glLoadMatrixf(glm::value_ptr(view));

        // Настройка освещения
        GLfloat light_position[] = {
            light.position.x, light.position.y, light.position.z, 1.0f
        };
        GLfloat light_ambient[] = {
            light.ambient.x, light.ambient.y, light.ambient.z, 1.0f
        };
        GLfloat light_diffuse[] = {
            light.diffuse.x, light.diffuse.y, light.diffuse.z, 1.0f
        };
        GLfloat light_specular[] = {
            light.specular.x, light.specular.y, light.specular.z, 1.0f
        };

        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

        // Отрисовка источника света (маленькая сфера)
        glPushMatrix();
        glTranslatef(light.position.x, light.position.y, light.position.z);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawSphere(0.1f, 10, 10);
        glEnable(GL_LIGHTING);
        glPopMatrix();

        // Отрисовка основной сферы
        glColor3f(0.7f, 0.2f, 0.2f);
        drawSphere(1.0f, 30, 30);

        window.display();
    }

    return 0;
}
