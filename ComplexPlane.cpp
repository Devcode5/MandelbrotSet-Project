#include "ComplexPlane.h"
#include <cmath>

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
    m_pixel_size = {pixelWidth, pixelHeight};
    m_aspectRatio = (float)pixelHeight / (float)pixelWidth;
    
    m_plane_center = {0, 0};
    m_plane_size = {BASE_WIDTH, BASE_HEIGHT * m_aspectRatio};
    m_zoomCount = 0;
    
    m_State = State::CALCULATING;

    // Initialize VertexArray as a list of Points
    m_vArray.setPrimitiveType(Points);
    m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(m_vArray);
}

void ComplexPlane::updateRender()
{
    // Only calculate if something changed (Zoom/Pan)
    if (m_State == State::CALCULATING)
    {
        for (int i = 0; i < m_pixel_size.y; i++) 
        {
            for (int j = 0; j < m_pixel_size.x; j++) 
            {
                // 1. Position the vertex on screen
                m_vArray[j + i * m_pixel_size.x].position = {(float)j, (float)i};

                // 2. Map pixel (j, i) to Complex Coordinate (r, i)
                Vector2f coord = mapPixelToCoords({j, i});

                // 3. Count how long it takes to diverge
                complex<double> c(coord.x, coord.y);
                size_t iter = countIterations(c);

                // 4. Color the pixel based on iterations
                Uint8 r, g, b;
                iterationsToRGB(iter, r, g, b);

                m_vArray[j + i * m_pixel_size.x].color = {r, g, b};
            }
        }
        m_State = State::DISPLAYING;
    }
}

size_t ComplexPlane::countIterations(complex<double> coord)
{
    complex<double> z = coord;
    size_t i = 0;

    // The Mandelbrot Algorithm
    // Stop if |z| > 2 (which means |z|^2 > 4) OR we hit the limit
    while (norm(z) < 4.0 && i < MAX_ITER)
    {
        z = z * z + coord;
        i++;
    }
    return i;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
    // If it stays inside the set forever, color it Black
    if (count == MAX_ITER)
    {
        r = 0; g = 0; b = 0;
    }
    else
    {
        // Simple colorful gradient for points outside the set
        // You can change these numbers to experiment with colors!
        if (count < 15)      { r = count * 15; g = 0; b = count * 5; } // Purpleish
        else if (count < 30) { r = 0; g = count * 8; b = count * 8; }  // Cyanish
        else if (count < 45) { r = 0; g = count * 5; b = 0; }          // Greenish
        else                 { r = count * 4; g = 0; b = 0; }          // Reddish
    }
}

void ComplexPlane::zoomIn()
{
    m_zoomCount++;
    float zoomFactor = pow(BASE_ZOOM, m_zoomCount);
    float newX = BASE_WIDTH * zoomFactor;
    float newY = BASE_HEIGHT * m_aspectRatio * zoomFactor;
    
    m_plane_size = {newX, newY};
    m_State = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
    m_zoomCount--;
    float zoomFactor = pow(BASE_ZOOM, m_zoomCount);
    float newX = BASE_WIDTH * zoomFactor;
    float newY = BASE_HEIGHT * m_aspectRatio * zoomFactor;
    
    m_plane_size = {newX, newY};
    m_State = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
    Vector2f coord = mapPixelToCoords(mousePixel);
    m_plane_center = coord;
    m_State = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
    m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
    stringstream ss;
    ss << "Mandelbrot Set" << endl;
    ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")" << endl;
    ss << "Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")" << endl;
    ss << "Left-Click: Zoom In | Right-Click: Zoom Out";
    text.setString(ss.str());
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
    // Determine the range of the current view
    float minX = m_plane_center.x - m_plane_size.x / 2.0f;
    float minY = m_plane_center.y - m_plane_size.y / 2.0f;
    float maxY = m_plane_center.y + m_plane_size.y / 2.0f;

    // Map X (0 to width) -> (minX to maxX)
    float offsetX = (float)mousePixel.x / m_pixel_size.x; 
    float real = minX + offsetX * m_plane_size.x;

    // Map Y (0 to height) -> (minY to maxY)
    // Note: We invert Y because screen Y goes down, but Graph Y goes up
    float offsetY = (float)mousePixel.y / m_pixel_size.y; 
    float imag = maxY - offsetY * m_plane_size.y; // Inverted mapping

    return {real, imag};
}
