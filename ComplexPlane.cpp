#include "ComplexPlane.h"
#include <cmath>
#include <complex>
//Created by Aqila Nasiry on 11/24/25.
ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
    m_pixel_size = {pixelWidth, pixelHeight};
    m_aspectRatio = (float)pixelHeight / pixelWidth;
    
    m_plane_center = {0, 0};
    m_plane_size = {BASE_WIDTH, BASE_HEIGHT * m_aspectRatio};
    m_zoomCount = 0;
    
    m_State = State::CALCULATING;

    m_vArray.setPrimitiveType(Points);
    m_vArray.resize(pixelWidth * pixelHeight);
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(m_vArray);
}

void ComplexPlane::updateRender()
{
    // The assignment requires us to check the state
    if (m_State == State::CALCULATING)
    {
        for (int i = 0; i < m_pixel_size.y; i++) 
        {
            for (int j = 0; j < m_pixel_size.x; j++) 
            {
                m_vArray[j + i * m_pixel_size.x].position = {(float)j, (float)i};

                Vector2f coord = mapPixelToCoords({j, i});
                

                complex<double> c(coord.x, coord.y);
                size_t iter = countIterations(c);

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


    while (abs(z) < 2.0 && i < MAX_ITER)
    {
        z = z * z + coord;
        i++;
    }
    return i;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{

    if (count == MAX_ITER)
    {
        r = 0;
        g = 0;
        b = 0;
    }
    else
    {

        if (count < 10)
        {
            r = 100; g = 0; b = 0; // Dark Red
        }
        else if (count < 20)
        {
            r = 255; g = 0; b = 0; // Red
        }
        else if (count < 30)
        {
            r = 0; g = 255; b = 0; // Green
        }
        else if (count < 40)
        {
            r = 0; g = 0; b = 255; // Blue
        }
        else
        {
            r = 255; g = 255; b = 255; // White
        }
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
    ss << "Left-Click for Zoom In" << endl;
    ss <<  "Right-Click for Zoom Out";
    text.setString(ss.str());
}

Vector2f ComplexPlane::mapPixelToCoords(const Vector2i& mousePixel) const
{
    float minX = m_plane_center.x - m_plane_size.x / 2.0f;
    float minY = m_plane_center.y - m_plane_size.y / 2.0f;
    float maxY = m_plane_center.y + m_plane_size.y / 2.0f;

    float offsetX = (float)mousePixel.x / m_pixel_size.x; 
    float real = minX + offsetX * m_plane_size.x;

    float offsetY = (float)mousePixel.y / m_pixel_size.y; 
    float imag = maxY - offsetY * m_plane_size.y; 

    return {real, imag};
}
