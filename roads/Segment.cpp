#include "Segment.h"

#include <cmath>

#include "Road.h"
#include "../rendering/AssetManager.h"


namespace Scale {
    constexpr float PPM = 32.0f; // Pixels Per Meter
    constexpr float LANE_WIDTH_METERS = 3.5f;
    constexpr float LANE_WIDTH_PX = LANE_WIDTH_METERS * PPM;
}


namespace Atlas {
    // Asphalt UV bounds
    constexpr float ASPHALT_U_START = 0.0f;
    constexpr float ASPHALT_U_END   = 0.25f;

    // Solid White Line UV bounds
    constexpr float SOLID_U_START   = 0.25f;
    constexpr float SOLID_U_END     = 0.35f;

    // Dashed White Line UV bounds
    constexpr float DASHED_U_START  = 0.35f;
    constexpr float DASHED_U_END    = 0.45f;
}


Segment::Segment(const sf::Vector2f start, const sf::Vector2f end, sf::Vector2f curvePoint) {
    // Default (straight) curve
    if (curvePoint == sf::Vector2f(0.0f, 0.0f)) {
        curvePoint = (start + end) / 2.0f;
    }

    mesh.setPrimitiveType(sf::TriangleStrip);

    const int laneCountTEMPORARY = 1;

    const float totalWidth = Scale::LANE_WIDTH_PX * laneCountTEMPORARY;
    const float halfWidth = totalWidth / 2.0f;

    float accumulatedDistance = 0.0f;
    sf::Vector2f previousCenter = start;

    // Texture dimensions
    const sf::Texture& texture = AssetManager::getInstance().getRoadTexture();
    const float textureWidth = static_cast<float>(texture.getSize().x);
    const float textureHeight = static_cast<float>(texture.getSize().y);

    for (int i = 0; i <= CURVEPOINTS; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(CURVEPOINTS);

        sf::Vector2f center = getBezierPoint(start, curvePoint, end, t);
        sf::Vector2f normal = getBezierNormal(start, curvePoint, end, t);

        if (i > 0) {
            const sf::Vector2f diff = center - previousCenter;
            accumulatedDistance += std::sqrt(diff.x * diff.x + diff.y * diff.y);
        }

        previousCenter = center;

        sf::Vector2f leftPos = center - normal * halfWidth;
        sf::Vector2f rightPos = center + normal * halfWidth;

        // Texture Mapping:
        const float uLeft = 0.0f;
        const float uRight = textureWidth;
        const float v = std::fmod(accumulatedDistance, textureHeight);

        // Triangle Strip row
        sf::Vertex leftVertex(leftPos, sf::Color::White, sf::Vector2f(uLeft, v));
        sf::Vertex rightVertex(rightPos, sf::Color::White, sf::Vector2f(uRight, v));

        mesh.append(leftVertex);
        mesh.append(rightVertex);
    }
}


sf::Vector2f Segment::getBezierPoint(const sf::Vector2f p0, const sf::Vector2f p1, const sf::Vector2f p2, const float t) const {
    const float u = 1.0f - t;
    return u * u * p0 + 2.0f * u * t * p1 + t * t * p2;
}


sf::Vector2f Segment::getBezierNormal(const sf::Vector2f p0, const sf::Vector2f p1, const sf::Vector2f p2, const float t) const {
    sf::Vector2f tangent = 2.0f * (1.0f - t) * (p1 - p0) + 2.0f * t * (p2 - p1);

    const float length = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
    if (length != 0) {
        tangent /= length;
    }

    return sf::Vector2f(-tangent.y, tangent.x);
}


void Segment::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = &AssetManager::getInstance().getRoadTexture();
    target.draw(mesh, states);
}
