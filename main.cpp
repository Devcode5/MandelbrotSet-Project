#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"
#include <iostream>

using namespace sf;
using namespace std;

int main()
{
    // Get Desktop Resolution and divide by 2 for a windowed size
    unsigned int width = VideoMode::getDesktopMode().width / 2;
    unsigned int height = VideoMode::getDesktopMode().height / 2;

    RenderWindow window(VideoMode(width, height), "Mandelbrot Set", Style::Default);
    
    ComplexPlane plane(width, height);

    Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "Error: Could not load arial.ttf" << endl;
        return -1;
    }

    Text text;
    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(Color::White);
    text.setPosition(10, 10);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            
            if (event.type == Event::MouseButtonPressed)
            {
                // Only register clicks inside the window
                Vector2i mousePos = Mouse::getPosition(window);
                if (mousePos.x >= 0 && mousePos.y >= 0 && 
                    mousePos.x < (int)width && mousePos.y < (int)height)
                {
                    plane.setCenter(mousePos);
                    
                    if (event.mouseButton.button == Mouse::Left)
                        plane.zoomIn();
                    else if (event.mouseButton.button == Mouse::Right)
                        plane.zoomOut();
                }
            }

            if (event.type == Event::MouseMoved)
            {
                plane.setMouseLocation(Mouse::getPosition(window));
            }
            
            if (Keyboard::isKeyPressed(Keyboard::Escape))
                window.close();
        }

        plane.updateRender();
        plane.loadText(text);

        window.clear();
        window.draw(plane);
        window.draw(text);
        window.display();
    }
    return 0;
}
