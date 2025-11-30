#ifndef COMPLEXPLANE_H
#define COMPLEXPLANE_H

#include <SFML/Graphics.hpp>
#include <complex>
#include <sstream>
#include <cmath>

const unsigned int MAX_ITER = 64;
const float BASE_WIDTH = 4.0f;
const float BASE_HEIGHT = 4.0f;
const float BASE_ZOOM = 0.5f;

enum class State { CALCULATING, DISPLAYING };

class ComplexPlane : public sf::Drawable {
public:
    ComplexPlane(int pixelWidth, int pixelHeight);

    // update and render control
    void updateRender();                 // recalculates vertex colors when CALCULATING
    void zoomIn();
    void zoomOut();
    void setCenter(sf::Vector2i mousePixel);
    void setMouseLocation(sf::Vector2i mousePixel);
    void loadText(sf::Text& text);        // fill text with center, mouse, size, zoom info

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // helpers required by the assignment
    sf::Vector2f mapPixelToCoords(const sf::Vector2i& pixel) const;
    size_t countIterations(const sf::Vector2f& coord) const;
    void iterationsToRGB(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b) const;

    // members
    int m_pixelWidth;
    int m_pixelHeight;
    float m_aspectRatio;                // height / width

    sf::Vector2f m_plane_center;        // real, imag
    sf::Vector2f m_plane_size;          // width, height in complex plane
    sf::Vector2f m_mouseLocation;       // complex coords under mouse (live)

    int m_zoomCount;
    State m_State;

    sf::VertexArray m_vArray;           // Point for each pixel
};
#endif // COMPLEXPLANE_H
