#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Создаем окно размером 800x600 пикселей
    sf::RenderWindow window(sf::VideoMode(800, 600), "Rectangle Transformations");
    window.setFramerateLimit(60);

    // Создаем прямоугольник
    sf::RectangleShape rectangle(sf::Vector2f(100.f, 50.f));
    rectangle.setFillColor(sf::Color::Green);
    rectangle.setPosition(400.f, 300.f); // Центр экрана
    rectangle.setOrigin(50.f, 25.f); // Устанавливаем центр вращения в центр прямоугольника

    // Параметры трансформации
    float moveSpeed = 5.0f;
    float rotationSpeed = 2.0f;
    float scaleSpeed = 0.02f;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Обработка клавиш для трансформаций
        // Перемещение
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            rectangle.move(-moveSpeed, 0);
            rectangle.setFillColor(sf::Color::Red);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            rectangle.move(moveSpeed, 0);
            rectangle.setFillColor(sf::Color::Blue);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            rectangle.move(0, -moveSpeed);
            rectangle.setFillColor(sf::Color::Yellow);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            rectangle.move(0, moveSpeed);
            rectangle.setFillColor(sf::Color::Magenta);
        }

        // Вращение
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            rectangle.rotate(-rotationSpeed);
            rectangle.setFillColor(sf::Color::Cyan);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            rectangle.rotate(rotationSpeed);
            rectangle.setFillColor(sf::Color::White);
        }

        // Масштабирование
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            rectangle.scale(1.0f - scaleSpeed, 1.0f - scaleSpeed);
            rectangle.setFillColor(sf::Color(255, 128, 0)); // Оранжевый
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
            rectangle.scale(1.0f + scaleSpeed, 1.0f + scaleSpeed);
            rectangle.setFillColor(sf::Color(128, 0, 255)); // Фиолетовый
        }

        // Если никакая клавиша не нажата, возвращаем зеленый цвет
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Q) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::E) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Z) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
            rectangle.setFillColor(sf::Color::Green);
        }

        window.clear(sf::Color(50, 50, 50)); // Тёмно-серый фон
        window.draw(rectangle);
        window.display();
    }

    return 0;
}