#ifndef COMPLEXPLANE_H
#define COMPLEXPLANE_H

#include <SFML/Graphics.hpp>
#include <complex>
#include <iostream>
#include <sstream>

using namespace sf;
using namespace std;

// Constants from the requirements
const unsigned int MAX_ITER = 64;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

enum class State { CALCULATING, DISPLAYING };

class ComplexPlane : public Drawable
{
public:
    // Constructor
    ComplexPlane(int pixelWidth, int pixelHeight);

    // Actions
    void draw(RenderTarget& target, RenderStates states) const override;
    void updateRender();
    void zoomIn();
    void zoomOut();
    void setCenter(Vector2i mousePixel);
    void setMouseLocation(Vector2i mousePixel);
    void loadText(Text& text);

private:
    // Internal Helper Functions
    size_t countIterations(complex<double> coord);
    void iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b);
    Vector2f mapPixelToCoords(Vector2i mousePixel);

    // Member Variables
    VertexArray m_vArray;
    State m_State;
    Vector2f m_mouseLocation;
    Vector2i m_pixel_size;
    Vector2f m_plane_center;
    Vector2f m_plane_size;
    int m_zoomCount;
    float m_aspectRatio;
};

#endif
