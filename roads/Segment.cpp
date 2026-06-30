#include <cmath>

#include "../rendering/AssetManager.h"
#include "Segment.h"
#include "Road.h"



namespace Scale {
    constexpr float PPM = 32.0f;
    constexpr float LANE_WIDTH_METERS = 3.5f;
    constexpr float LANE_WIDTH_PX = LANE_WIDTH_METERS * PPM;
}


namespace Atlas {
    // Asphalt: Pixels 0 to 256 (256px chunk)
    constexpr float ASPHALT_U_START = 0.0f;
    constexpr float ASPHALT_U_END   = 256.0f;

    // Solid Line: Pixels 256 to 288 (32px chunk)
    constexpr float SOLID_U_START   = 256.0f;
    constexpr float SOLID_U_END     = 288.0f;

    // Dashed Line: Pixels 288 to 320 (32px chunk)
    constexpr float DASHED_U_START  = 288.0f;
    constexpr float DASHED_U_END    = 320.0f;
}


Segment::Segment(
    const sf::Vector2f start, const sf::Vector2f end, const sf::Vector2f curvePoint,
    const std::vector<LaneConfig>& lanes,
    const std::vector<MarkingConfig>& markings)
        : start(start), end(end), curvePoint(curvePoint)
{
    asphaltMesh.setPrimitiveType(sf::Triangles);
    markingsMesh.setPrimitiveType(sf::Triangles);

    precalculateDistances();

    this->generateAsphaltMesh(lanes);
    this->generateMarkingsMesh(markings);
}

void Segment::precalculateDistances() {
    mDistances.resize(CURVEPOINTS + 1, 0.0f);
    float accumulated = 0.0f;
    sf::Vector2f prevPoint = start;

    for (int i = 0; i <= CURVEPOINTS; ++i) {
        float t = static_cast<float>(i) / CURVEPOINTS;
        sf::Vector2f currentPoint = getBezierPoint(start, curvePoint, end, t);

        if (i > 0) {
            sf::Vector2f diff = currentPoint - prevPoint;
            accumulated += std::sqrt(diff.x * diff.x + diff.y * diff.y);
        }
        mDistances[i] = accumulated;
        prevPoint = currentPoint;
    }
}


void Segment::generateAsphaltMesh(const std::vector<LaneConfig>& lanes) {
    float totalWidthPx = 0.0f;

    for (const auto& [widthMeters] : lanes) totalWidthPx += widthMeters * Scale::PPM;

    float currentLeftOffset = -totalWidthPx / 2.0f;

    for (const auto& [widthMeters] : lanes) {
        const float laneWidthPx = widthMeters * Scale::PPM;
        const float laneRightOffset = currentLeftOffset + laneWidthPx;

        // Cache previous vertices to draw complete quads
        sf::Vector2f prevLeftPos;
        sf::Vector2f prevRightPos;
        float prevV = 0.0f;

        for (int i = 0; i <= CURVEPOINTS; ++i) {
            float t = static_cast<float>(i) / CURVEPOINTS;
            sf::Vector2f center = getBezierPoint(start, curvePoint, end, t);
            sf::Vector2f normal = getBezierNormal(start, curvePoint, end, t);
            float v = mDistances[i];

            sf::Vector2f leftPos  = center + normal * currentLeftOffset;
            sf::Vector2f rightPos = center + normal * laneRightOffset;

            if (i > 0) {
                // Triangle 1 (Top-Left, Top-Right, Bottom-Left)
                asphaltMesh.append(sf::Vertex(prevLeftPos, sf::Color::White, sf::Vector2f(Atlas::ASPHALT_U_START, prevV)));
                asphaltMesh.append(sf::Vertex(prevRightPos, sf::Color::White, sf::Vector2f(Atlas::ASPHALT_U_END, prevV)));
                asphaltMesh.append(sf::Vertex(leftPos, sf::Color::White, sf::Vector2f(Atlas::ASPHALT_U_START, v)));

                // Triangle 2 (Top-Right, Bottom-Right, Bottom-Left)
                asphaltMesh.append(sf::Vertex(prevRightPos, sf::Color::White, sf::Vector2f(Atlas::ASPHALT_U_END, prevV)));
                asphaltMesh.append(sf::Vertex(rightPos, sf::Color::White, sf::Vector2f(Atlas::ASPHALT_U_END, v)));
                asphaltMesh.append(sf::Vertex(leftPos, sf::Color::White, sf::Vector2f(Atlas::ASPHALT_U_START, v)));
            }

            prevLeftPos = leftPos;
            prevRightPos = rightPos;
            prevV = v;
        }
        currentLeftOffset = laneRightOffset;
    }
}

void Segment::generateMarkingsMesh(const std::vector<MarkingConfig>& markings) {
    for (const auto& [offsetFromRoadCenterMeters, markingType] : markings) {
        float uStart = 0.0f, uEnd = 0.0f;

        if (markingType == NONE) continue;

        if (markingType == SOLID) {
            uStart = Atlas::SOLID_U_START;
            uEnd = Atlas::SOLID_U_END;
        } else if (markingType == DASHED) {
            uStart = Atlas::DASHED_U_START;
            uEnd = Atlas::DASHED_U_END;
        }

        const float centerOffsetPx = offsetFromRoadCenterMeters * Scale::PPM;
        const float halfMarkingWidthPx = 2.0f;

        const float leftLineOffset = centerOffsetPx - halfMarkingWidthPx;
        const float rightLineOffset = centerOffsetPx + halfMarkingWidthPx;

        // Variables to store the previous step's data
        sf::Vector2f prevLeftPos;
        sf::Vector2f prevRightPos;
        float prevV = 0.0f;

        for (int i = 0; i <= CURVEPOINTS; ++i) {
            float t = static_cast<float>(i) / CURVEPOINTS;
            sf::Vector2f center = getBezierPoint(start, curvePoint, end, t);
            sf::Vector2f normal = getBezierNormal(start, curvePoint, end, t);
            float v = mDistances[i];

            sf::Vector2f leftPos  = center + normal * leftLineOffset;
            sf::Vector2f rightPos = center + normal * rightLineOffset;

            if (i > 0) {
                // Triangle 1 (Top-Left, Top-Right, Bottom-Left)
                markingsMesh.append(sf::Vertex(prevLeftPos, sf::Color::White, sf::Vector2f(uStart, prevV)));
                markingsMesh.append(sf::Vertex(prevRightPos, sf::Color::White, sf::Vector2f(uEnd, prevV)));
                markingsMesh.append(sf::Vertex(leftPos, sf::Color::White, sf::Vector2f(uStart, v)));

                // Triangle 2 (Top-Right, Bottom-Right, Bottom-Left)
                markingsMesh.append(sf::Vertex(prevRightPos, sf::Color::White, sf::Vector2f(uEnd, prevV)));
                markingsMesh.append(sf::Vertex(rightPos, sf::Color::White, sf::Vector2f(uEnd, v)));
                markingsMesh.append(sf::Vertex(leftPos, sf::Color::White, sf::Vector2f(uStart, v)));
            }

            // Cache current vertices for the next loop iteration
            prevLeftPos = leftPos;
            prevRightPos = rightPos;
            prevV = v;
        }
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

    target.draw(asphaltMesh, states);
    target.draw(markingsMesh, states);
}
