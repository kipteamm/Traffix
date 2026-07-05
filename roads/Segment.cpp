#include <optional>
#include <cmath>

#include "RoadNetwork.h"
#include "Segment.h"
#include "Bezier.h"



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

    constexpr float CENTER_U = (ASPHALT_U_START + ASPHALT_U_END) / 2.0f;
}


Segment::Segment(
    Node* start, Node* end, const sf::Vector2f curvePoint,
    const RoadConfig& config
)
        : start(start), end(end), curvePoint(curvePoint), config(config)
{
    asphaltMesh.setPrimitiveType(sf::Triangles);
    markingsMesh.setPrimitiveType(sf::Triangles);

    for (const auto& lane : config.lanes) {
        this->laneWidth += lane.widthMeters;
    }

    precalculateDistances();

    this->generateAsphaltMesh();
    this->generateMarkingsMesh();
}


void Segment::setVOffset(const float offset) {
    m_vOffset = offset;
}


float Segment::getLength() const {
    if (mDistances.empty()) return 0.0f;
    return mDistances.back();
}


void Segment::setStartSetback(const float distance) {
    startSetback = distance;
}

void Segment::setEndSetback(const float distance) {
    endSetback = distance;
}


void Segment::updateNormals(const std::optional<sf::Vector2f> startNormal, const std::optional<sf::Vector2f> endNormal) {
    customStartNormal = startNormal;
    customEndNormal = endNormal;

    asphaltMesh.clear();
    markingsMesh.clear();

    generateAsphaltMesh();
    generateMarkingsMesh();
}



void Segment::precalculateDistances() {
    mDistances.resize(CURVEPOINTS + 1, 0.0f);
    float accumulated = 0.0f;
    sf::Vector2f prevPoint = start->position;

    for (int i = 0; i <= CURVEPOINTS; ++i) {
        float t = static_cast<float>(i) / CURVEPOINTS;
        sf::Vector2f currentPoint = getBezierPoint(start->position, curvePoint, end->position, t);

        if (i > 0) {
            sf::Vector2f diff = currentPoint - prevPoint;
            accumulated += std::sqrt(diff.x * diff.x + diff.y * diff.y);
        }
        mDistances[i] = accumulated;
        prevPoint = currentPoint;
    }
}


float Segment::getT(const float distance) const {
    if (distance <= 0.0f) return 0.0f;
    if (mDistances.empty()) return 0.0f;
    if (distance >= mDistances.back()) return 1.0f;

    for (size_t i = 0; i < mDistances.size() - 1; ++i) {
        // if (targetDist >= mDistances[i] && targetDist <= mDistances[i+1]) {
        if (distance < mDistances[i] || distance > mDistances[i+1]) continue;

        const float segmentLen = mDistances[i+1] - mDistances[i];
        const float fraction = (segmentLen == 0.0f) ? 0.0f : (distance - mDistances[i]) / segmentLen;

        return (i + fraction) / CURVEPOINTS;
    }

    return 1.0f;
}



void Segment::generateAsphaltMesh() {
    float totalWidthPx = 0.0f;
    for (const auto& [widthMeters] : config.lanes) totalWidthPx += widthMeters * Scale::PPM;

    float currentLeftOffset = -totalWidthPx / 2.0f;

    const float totalLength = getLength();
    // const float startT = getT(startSetback);

    for (const auto& [widthMeters] : config.lanes) {
        const float laneWidthPx = widthMeters * Scale::PPM;
        const float laneRightOffset = currentLeftOffset + laneWidthPx;

        sf::Vector2f prevLeftPos;
        sf::Vector2f prevRightPos;
        float prevV = 0.0f;
        bool prevCutoff = true;

        // float currentDist = startSetback;
        // sf::Vector2f prevCenter = getBezierPoint(start->position, curvePoint, end->position, startT);

        for (int i = 0; i <= CURVEPOINTS; ++i) {
            const float currentDist = mDistances[i];

            // Determine whether we should append triangles to mesh. Triangles
            // in setback zone should be ignored.
            const bool cutoff = (currentDist < startSetback) || (currentDist > (totalLength - endSetback));

            float t = static_cast<float>(i) / CURVEPOINTS;
            sf::Vector2f center = getBezierPoint(start->position, curvePoint, end->position, t);

            sf::Vector2f baseNormal = getBezierNormal(start->position, curvePoint, end->position, t); // Keep track of base normal length (1.0)
            sf::Vector2f normal = baseNormal;

            float uScale = 1.0f; // Default scale factor

            if (i == 0 && customStartNormal.has_value()) {
                normal = customStartNormal.value();
                // The length of the miter normal dictates the scale factor
                uScale = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            } else if (i == CURVEPOINTS && customEndNormal.has_value()) {
                normal = customEndNormal.value();
                uScale = std::sqrt(normal.x * normal.x + normal.y * normal.y);
            }

            const float v = m_vOffset + mDistances[i];

            sf::Vector2f leftPos  = center + normal * currentLeftOffset;
            sf::Vector2f rightPos = center + normal * laneRightOffset;

            if (i > 0 && !cutoff && !prevCutoff) {
                // Scale the U coordinates relative to the center line mapping offset based on uScale
                sf::Vector2f uvPrevLeft(Atlas::CENTER_U + (currentLeftOffset * (i == 1 && customStartNormal.has_value() ? std::sqrt(customStartNormal.value().x*customStartNormal.value().x + customStartNormal.value().y*customStartNormal.value().y) : 1.0f)), prevV);
                sf::Vector2f uvPrevRight(Atlas::CENTER_U + (laneRightOffset * (i == 1 && customStartNormal.has_value() ? std::sqrt(customStartNormal.value().x*customStartNormal.value().x + customStartNormal.value().y*customStartNormal.value().y) : 1.0f)), prevV);
                sf::Vector2f uvLeft(Atlas::CENTER_U + (currentLeftOffset * uScale), v);
                sf::Vector2f uvRight(Atlas::CENTER_U + (laneRightOffset * uScale), v);

                // Triangle 1
                asphaltMesh.append(sf::Vertex(prevLeftPos, sf::Color::White, uvPrevLeft));
                asphaltMesh.append(sf::Vertex(prevRightPos, sf::Color::White, uvPrevRight));
                asphaltMesh.append(sf::Vertex(leftPos, sf::Color::White, uvLeft));

                // Triangle 2
                asphaltMesh.append(sf::Vertex(prevRightPos, sf::Color::White, uvPrevRight));
                asphaltMesh.append(sf::Vertex(rightPos, sf::Color::White, uvRight));
                asphaltMesh.append(sf::Vertex(leftPos, sf::Color::White, uvLeft));
            }

            prevLeftPos = leftPos;
            prevRightPos = rightPos;
            prevV = v;
            prevCutoff = cutoff;
        }
        currentLeftOffset = laneRightOffset;
    }
}


void Segment::generateMarkingsMesh() {
    const float totalLength = getLength();

    for (const auto& [offsetFromRoadCenterMeters, markingType] : config.markings) {
        float uStart = 0.0f, uEnd = 0.0f;

        if (markingType == NONE) continue;

        const float halfMarkingWidthPx = 2.0f;

        if (markingType == SOLID) {
            float uCenter = (Atlas::SOLID_U_START + Atlas::SOLID_U_END) / 2.0f;
            uStart = uCenter - halfMarkingWidthPx;
            uEnd   = uCenter + halfMarkingWidthPx;
        } else if (markingType == DASHED) {
            float uCenter = (Atlas::DASHED_U_START + Atlas::DASHED_U_END) / 2.0f;
            uStart = uCenter - halfMarkingWidthPx;
            uEnd   = uCenter + halfMarkingWidthPx;
        }

        const float centerOffsetPx = offsetFromRoadCenterMeters * Scale::PPM;
        const float leftLineOffset = centerOffsetPx - halfMarkingWidthPx;
        const float rightLineOffset = centerOffsetPx + halfMarkingWidthPx;

        sf::Vector2f prevLeftPos;
        sf::Vector2f prevRightPos;
        float prevV = 0.0f;
        bool prevCutoff = true;

        for (int i = 0; i <= CURVEPOINTS; ++i) {
            const float currentDist = mDistances[i];

            // Determine whether we should append triangles to mesh. Triangles
            // in setback zone should be ignored.
            const bool cutoff = (currentDist < startSetback) || (currentDist > (totalLength - endSetback));

            float t = static_cast<float>(i) / CURVEPOINTS;
            sf::Vector2f center = getBezierPoint(start->position, curvePoint, end->position, t);

            // Base normal fallback
            sf::Vector2f normal = getBezierNormal(start->position, curvePoint, end->position, t);

            // Miter override injections
            if (i == 0 && customStartNormal.has_value()) {
                normal = customStartNormal.value();
            } else if (i == CURVEPOINTS && customEndNormal.has_value()) {
                normal = customEndNormal.value();
            }

            const float v = m_vOffset + mDistances[i];

            sf::Vector2f leftPos  = center + normal * leftLineOffset;
            sf::Vector2f rightPos = center + normal * rightLineOffset;

            if (i > 0 && !cutoff && !prevCutoff) {
                // Triangle 1
                markingsMesh.append(sf::Vertex(prevLeftPos, sf::Color::White, sf::Vector2f(uStart, prevV)));
                markingsMesh.append(sf::Vertex(prevRightPos, sf::Color::White, sf::Vector2f(uEnd, prevV)));
                markingsMesh.append(sf::Vertex(leftPos, sf::Color::White, sf::Vector2f(uStart, v)));

                // Triangle 2
                markingsMesh.append(sf::Vertex(prevRightPos, sf::Color::White, sf::Vector2f(uEnd, prevV)));
                markingsMesh.append(sf::Vertex(rightPos, sf::Color::White, sf::Vector2f(uEnd, v)));
                markingsMesh.append(sf::Vertex(leftPos, sf::Color::White, sf::Vector2f(uStart, v)));
            }

            prevLeftPos = leftPos;
            prevRightPos = rightPos;
            prevV = v;
            prevCutoff = cutoff;
        }
    }
}
