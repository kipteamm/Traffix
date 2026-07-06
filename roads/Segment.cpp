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

    // Arrow: Pixels 320 to 352 (64px chunk)
    constexpr float ARROW_U_START = 320.0f;
    constexpr float ARROW_U_END   = 352.0f;
    constexpr float ARROW_V_SIZE  = 64.0f;

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


void Segment::generateAsphaltMesh() {
    const float totalWidthPx = laneWidth * Scale::PPM;
    float currentLeftOffset = -totalWidthPx / 2.0f;

    const float startT = getT(startSetback);
    const float endT = getT(getLength() - endSetback);

    for (const auto& lane : config.lanes) {
        const float laneWidthPx = lane.widthMeters * Scale::PPM;
        const float laneRightOffset = currentLeftOffset + laneWidthPx;

        sf::Vector2f prevLeftPos;
        sf::Vector2f prevRightPos;
        float prevV = 0.0f;

        float currentDist = startSetback;
        sf::Vector2f prevCenter = getBezierPoint(start->position, curvePoint, end->position, startT);

        for (int i = 0; i <= CURVEPOINTS; ++i) {
            float t = startT + (static_cast<float>(i) / CURVEPOINTS) * (endT - startT);
            sf::Vector2f center = getBezierPoint(start->position, curvePoint, end->position, t);

            currentDist += std::hypot(center.x - prevCenter.x, center.y - prevCenter.y);
            prevCenter = center;

            sf::Vector2f baseNormal = getBezierNormal(start->position, curvePoint, end->position, t);
            sf::Vector2f normal = baseNormal;

            float uScale = 1.0f;

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

            if (i > 0) {
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
        }
        currentLeftOffset = laneRightOffset;
    }
}


void Segment::generateMarkingsMesh() {
    const float startT = getT(startSetback);
    const float endT = getT(getLength() - endSetback);

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

        float currentDist = startSetback;
        sf::Vector2f prevCenter = getBezierPoint(start->position, curvePoint, end->position, startT);


        for (int i = 0; i <= CURVEPOINTS; ++i) {
            float t = startT + (static_cast<float>(i) / CURVEPOINTS) * (endT - startT);
            sf::Vector2f center = getBezierPoint(start->position, curvePoint, end->position, t);

            currentDist += std::hypot(center.x - prevCenter.x, center.y - prevCenter.y);
            prevCenter = center;

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

            if (i > 0) {
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
        }
    }

    // Directional arrows are only required for single lane roads
    if (config.lanes.size() > 1) return;

    generateArrowMesh();
}


constexpr float ARROW_WIDTH = 32.0f;
constexpr float ARROW_LENGTH = 64.0f;


void Segment::generateArrowMesh() {
    // Start the first arrow 20 meters in so it isn't exactly on the intersection
    float distanceCursor = 10.0f * Scale::PPM;

    const float totalWidthPx = laneWidth * Scale::PPM;

    while (distanceCursor < getLength() - endSetback) {
        float currentLeftOffset = -totalWidthPx / 2.0f;

        // Get the 't' value (0.0 to 1.0) along the Bezier curve for this specific distance
        float t = getT(distanceCursor);

        sf::Vector2f centerPoint = getBezierPoint(start->position, curvePoint, end->position, t);
        sf::Vector2f normal = getBezierNormal(start->position, curvePoint, end->position, t);

        // Tangent is perpendicular to the normal (normal.y, -normal.x)
        sf::Vector2f baseTangent(normal.y, -normal.x);

        // Place an arrow in each lane
        for (const auto& lane : config.lanes) {
            const float laneWidthPx = lane.widthMeters * Scale::PPM;
            const float laneCenterOffset = currentLeftOffset + (laneWidthPx / 2.0f);

            sf::Vector2f tangent = baseTangent;

            // Flip the arrow around if the lane goes backward
            if (lane.direction == BACKWARD) {
                tangent = -tangent;
            }

            // Find the exact center of this specific lane
            sf::Vector2f laneCenterPoint = centerPoint + normal * laneCenterOffset;

            // Calculate the vectors to push out the corners of our rectangle
            const sf::Vector2f halfWidthVec = normal * (ARROW_WIDTH / 2.0f);
            const sf::Vector2f halfLengthVec = tangent * (ARROW_LENGTH / 2.0f);

            // Calculate the 4 corners of the rotated quad (rectangle)
            sf::Vector2f topLeft = laneCenterPoint - halfWidthVec + halfLengthVec;
            sf::Vector2f topRight = laneCenterPoint + halfWidthVec + halfLengthVec;
            sf::Vector2f bottomLeft = laneCenterPoint - halfWidthVec - halfLengthVec;
            sf::Vector2f bottomRight = laneCenterPoint + halfWidthVec - halfLengthVec;

            // UVs based on your Atlas (Assuming you added ARROW_U_START, etc.)
            sf::Vector2f uvTopLeft(Atlas::ARROW_U_START, 0.0f);
            sf::Vector2f uvTopRight(Atlas::ARROW_U_END, 0.0f);
            sf::Vector2f uvBottomLeft(Atlas::ARROW_U_START, Atlas::ARROW_V_SIZE);
            sf::Vector2f uvBottomRight(Atlas::ARROW_U_END, Atlas::ARROW_V_SIZE);

            // Append the two triangles directly to the markingsMesh
            markingsMesh.append(sf::Vertex(topLeft, sf::Color::White, uvTopLeft));
            markingsMesh.append(sf::Vertex(topRight, sf::Color::White, uvTopRight));
            markingsMesh.append(sf::Vertex(bottomLeft, sf::Color::White, uvBottomLeft));

            markingsMesh.append(sf::Vertex(topRight, sf::Color::White, uvTopRight));
            markingsMesh.append(sf::Vertex(bottomRight, sf::Color::White, uvBottomRight));
            markingsMesh.append(sf::Vertex(bottomLeft, sf::Color::White, uvBottomLeft));

            // Move the offset for the next lane
            currentLeftOffset += laneWidthPx;
        }

        // Move forward by our fixed interval
        distanceCursor += Scale::ARROW_SPACING_METERS * Scale::PPM;
    }
}

