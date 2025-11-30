#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"
#include <iostream>

int main() {
    // get desktop resolution and use half resolution for speed in VM
    sf::VideoMode dm = sf::VideoMode::getDesktopMode();
    int width = static_cast<int>(dm.width / 2);
    int height = static_cast<int>(dm.height / 2);

    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(width),
                                          static_cast<unsigned int>(height)),
                            "Mandelbrot Set");

    window.setFramerateLimit(30);

    // load a font available on most Linux installs; if not present, program still runs but text may not show
    sf::Font font;
    bool fontLoaded = true;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // try local path fallback (if you put a TTF in project)
        if (!font.loadFromFile("assets/arial.ttf")) {
            std::cerr << "Warning: could not load font. Text will not appear.\n";
            fontLoaded = false;
        }
    }

    sf::Text infoText;
    if (fontLoaded) {
        infoText.setFont(font);
        infoText.setCharacterSize(14);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(8.f, 8.f);
    }

    // create the complex plane object (half-resolution)
    ComplexPlane complexPlane(width, height);

    // main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // window closed
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // mouse button pressed
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (event.mouseButton.button == sf::Mouse::Left) {
                    complexPlane.setCenter(mousePos);
                    complexPlane.zoomIn();
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    complexPlane.setCenter(mousePos);
                    complexPlane.zoomOut();
                }
            }

            // mouse moved: update live mouse position
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos(event.mouseMove.x, event.mouseMove.y);
                complexPlane.setMouseLocation(mousePos);
            }
        }

        // escape => close
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

        // update and draw
        complexPlane.updateRender();
        if (fontLoaded) complexPlane.loadText(infoText);

        window.clear(sf::Color::Black);
        window.draw(complexPlane);
        if (fontLoaded) window.draw(infoText);
        window.display();
    }

    return 0;
}

