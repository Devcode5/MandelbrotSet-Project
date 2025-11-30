#include "ComplexPlane.h"
#include <iostream>

// small helper: convert HSL-like hue to RGB (not full HSL, simple)
static void hueToRGB(float hue, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b) {
    // hue in [0,360)
    float h = fmodf(hue, 360.0f);
    float c = 1.0f;
    float x = 1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f);
    float rp=0, gp=0, bp=0;
    if (h < 60)      { rp = c; gp = x; bp = 0; }
    else if (h < 120){ rp = x; gp = c; bp = 0; }
    else if (h < 180){ rp = 0; gp = c; bp = x; }
    else if (h < 240){ rp = 0; gp = x; bp = c; }
    else if (h < 300){ rp = x; gp = 0; bp = c; }
    else             { rp = c; gp = 0; bp = x; }

    // map [0,1] to [0,255], apply slight gamma/scale for nicer brightness
    r = static_cast<sf::Uint8>(std::round((rp) * 255.0f));
    g = static_cast<sf::Uint8>(std::round((gp) * 255.0f));
    b = static_cast<sf::Uint8>(std::round((bp) * 255.0f));
}

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
: m_pixelWidth(pixelWidth),
  m_pixelHeight(pixelHeight),
  m_vArray(sf::Points)
{
    m_aspectRatio = static_cast<float>(m_pixelHeight) / static_cast<float>(m_pixelWidth);
    m_plane_center = { 0.0f, 0.0f };
    m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };
    m_zoomCount = 0;
    m_State = State::CALCULATING;
    m_mouseLocation = {0.f, 0.f};

    m_vArray.setPrimitiveType(sf::Points);
    m_vArray.resize(static_cast<size_t>(m_pixelWidth) * static_cast<size_t>(m_pixelHeight));
}

void ComplexPlane::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_vArray);
}

void ComplexPlane::updateRender() {
    if (m_State != State::CALCULATING) return;

    // double loop: i -> y (rows), j -> x (columns)
    for (int i = 0; i < m_pixelHeight; ++i) {
        for (int j = 0; j < m_pixelWidth; ++j) {
            int idx = j + i * m_pixelWidth;
            // position of the vertex (pixel)
            m_vArray[idx].position = sf::Vector2f(static_cast<float>(j), static_cast<float>(i));

            // map pixel -> complex coordinate
            sf::Vector2f coord = mapPixelToCoords({ j, i });

            // compute iterations
            size_t iter = countIterations(coord);

            // color mapping
            sf::Uint8 r, g, b;
            iterationsToRGB(iter, r, g, b);
            m_vArray[idx].color = sf::Color(r, g, b);
        }
    }

    m_State = State::DISPLAYING;
}

void ComplexPlane::zoomIn() {
    ++m_zoomCount;
    float factor = std::pow(BASE_ZOOM, static_cast<float>(m_zoomCount));
    m_plane_size.x = BASE_WIDTH * factor;
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * factor;
    m_State = State::CALCULATING;
}

void ComplexPlane::zoomOut() {
    --m_zoomCount;
    float factor = std::pow(BASE_ZOOM, static_cast<float>(m_zoomCount));
    m_plane_size.x = BASE_WIDTH * factor;
    m_plane_size.y = BASE_HEIGHT * m_aspectRatio * factor;
    m_State = State::CALCULATING;
}

void ComplexPlane::setCenter(sf::Vector2i mousePixel) {
    m_plane_center = mapPixelToCoords(mousePixel);
    m_State = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(sf::Vector2i mousePixel) {
    m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(sf::Text& text) {
    std::ostringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(6);
    ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")\n";
    ss << "Mouse:  (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")\n";
    ss << "Plane size: (" << m_plane_size.x << " x " << m_plane_size.y << ")\n";
    ss << "Zoom count: " << m_zoomCount;
    text.setString(ss.str());
}

size_t ComplexPlane::countIterations(const sf::Vector2f& coord) const {
    // start z = c per the assignment sample
    std::complex<double> c(static_cast<double>(coord.x), static_cast<double>(coord.y));
    std::complex<double> z = c;
    size_t i = 0;
    // while magnitude < 2 and i < MAX_ITER
    while (std::abs(z) < 2.0 && i < MAX_ITER) {
        z = z * z + c;
        ++i;
    }
    return i;
}

void ComplexPlane::iterationsToRGB(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b) const {
    if (count >= MAX_ITER) {
        // inside set -> black
        r = g = b = 0;
        return;
    }

    // Color scheme: split into 5 regions across iteration counts for a pleasing gradient.
    float t = static_cast<float>(count) / static_cast<float>(MAX_ITER); // 0..1
    // We'll map to hues: purple(270) -> turquoise(180) -> green(120) -> yellow(60) -> red(0)
    float hue;
    if (t < 0.2f) {
        // purple -> blue range
        float local = t / 0.2f;
        hue = 270.0f - local * 90.0f; // 270 -> 180
    } else if (t < 0.4f) {
        float local = (t - 0.2f) / 0.2f;
        hue = 180.0f - local * 60.0f; // 180 -> 120
    } else if (t < 0.6f) {
        float local = (t - 0.4f) / 0.2f;
        hue = 120.0f - local * 60.0f; // 120 -> 60
    } else {
        float local = (t - 0.6f) / 0.4f;
        hue = 60.0f - local * 60.0f; // 60 -> 0
    }
    hue = std::max(0.0f, std::min(360.0f, hue));
    hueToRGB(hue, r, g, b);
}
