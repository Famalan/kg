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
        position(glm::vec3(0.0f, 0.0f, 15.0f)),
        front(glm::vec3(0.0f, 0.0f, -1.0f)),
        up(glm::vec3(0.0f, 1.0f, 0.0f)),
        yaw(-90.0f),
        pitch(0.0f),
        speed(2.5f),
        sensitivity(0.05f) {}
};

// Глобальные переменные
Camera camera;
bool firstMouse = true;
float lastX = 400.0f;
float lastY = 300.0f;
float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;
float fov = 45.0f; // Для изменения угла обзора

void processInput(sf::Window& window, float deltaTime) {
    // Перемещение камеры
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

    // Вращение вокруг осей
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        rotationY -= 2.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        rotationY += 2.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        rotationX -= 2.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        rotationX += 2.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        rotationZ -= 2.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        rotationZ += 2.0f;

    // Изменение угла обзора (FOV)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        fov = std::max(1.0f, fov - 1.0f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        fov = std::min(90.0f, fov + 1.0f);
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

    // Уменьшаем чувствительность для трекпада
    camera.sensitivity = 0.05f;  // Было 0.1f

    xoffset *= camera.sensitivity;
    yoffset *= camera.sensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    // Ограничение углов поворота камеры
    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    // Обновление векторов направления камеры
    glm::vec3 front;
    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(front);
}

void drawPyramid() {
    glBegin(GL_TRIANGLES);
    
    // Передняя грань
    glNormal3f(0.0f, 0.5f, 1.0f);
    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    
    // Правая грань
    glNormal3f(1.0f, 0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f); // Синий
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    
    // Задняя грань
    glNormal3f(0.0f, 0.5f, -1.0f);
    glColor3f(1.0f, 1.0f, 0.0f); // Желтый
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    
    // Левая грань
    glNormal3f(-1.0f, 0.5f, 0.0f);
    glColor3f(1.0f, 0.0f, 1.0f); // Пурпурный
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    
    glEnd();

    // Основание (квадрат)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);  // Нормаль для основания
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glEnd();
}

void drawCube() {
    glBegin(GL_QUADS);
    
    // Передняя грань
    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Задняя грань (зеленая)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Верхняя грань (синяя)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Нижняя грань (желтая)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Правая грань (пурпурная)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Левая грань (голубая)
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    glEnd();
}

int main() {
    // Создание окна SFML с поддержкой OpenGL
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 2; // Изменено на 2
    settings.minorVersion = 1; // Изменено на 1

    sf::Window window(sf::VideoMode(800, 600), "3D Camera Demo", 
                     sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    window.setMouseCursorVisible(false);

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        std::cout << "Failed to initialize GLEW: " 
                  << glewGetErrorString(glewStatus) << std::endl;
        return -1;
    }

    // Проверка версии OpenGL
    std::cout << "OpenGL version: " 
              << glGetString(GL_VERSION) << std::endl;

    // Настройка OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);    // Включаем освещение
    glEnable(GL_LIGHT0);      // Включаем первый источник света
    glEnable(GL_COLOR_MATERIAL); // Включаем материалы
    glViewport(0, 0, 800, 600);

    // Настройка света
    GLfloat light_position[] = { 0.0f, 10.0f, 10.0f, 1.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    // Убираем захват курсора и его центрирование
    window.setMouseCursorGrabbed(false);
    window.setMouseCursorVisible(true);

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

        // Оработка ввода
        processInput(window, deltaTime);

        // Очистка буферов
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Черный фон
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Обновляем соотношение сторон для текущего размера окна
        float aspectRatio = static_cast<float>(window.getSize().x) / 
                           static_cast<float>(window.getSize().y);

        // Настройка матриц проекции и вида
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glm::mat4 projection = glm::perspective(
            glm::radians(fov),    // Используем переменную fov
            aspectRatio,          
            0.1f,                 
            100.0f               
        );
        glLoadMatrixf(glm::value_ptr(projection));

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // Установка камеры
        glm::mat4 view = glm::lookAt(camera.position,
                                    camera.position + camera.front,
                                    camera.up);
        glLoadMatrixf(glm::value_ptr(view));

        // Отрисовка объектов
        glPushMatrix();
        glTranslatef(-4.0f, 0.0f, 0.0f);
        glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
        glRotatef(rotationZ, 0.0f, 0.0f, 1.0f);
        drawPyramid();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(4.0f, 0.0f, 0.0f);
        drawCube();
        glPopMatrix();

        // Проверка на ошибки OpenGL
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cout << "OpenGL error: " << err << std::endl;
        }

        // Обмен буферов
        window.display();
    }

    return 0;
} 
