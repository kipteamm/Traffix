#include "RoadBuilder.h"

#include <iostream>
#include <optional>
#include <memory>
#include <cmath>


// Snapping radius in pixels
constexpr float SNAPPING_DISTANCE = 30.f;


RoadBuilder::RoadBuilder() {
    // At most 3 points (start, curve, end)
    points.reserve(3);
}


void RoadBuilder::addPoint() {
    points.push_back(std::move(currentMousePos));
}


bool RoadBuilder::popPoint() {
    if (points.empty()) return false;

    points.pop_back();
    return true;
}



void RoadBuilder::setMode(const RoadBuildMode newMode) {
    this->mode = newMode;
}


sf::Vector2f calculateMiterNormal(const sf::Vector2f tangentA, const sf::Vector2f tangentB) {
    // Get the average tangent vector
    sf::Vector2f avgTangent = tangentA + tangentB;

    // Handle perfectly flat/collinear or 180-degree turn edge-cases safely
    const float len = std::sqrt(avgTangent.x * avgTangent.x + avgTangent.y * avgTangent.y);
    if (len == 0.0f)
        return sf::Vector2f(-tangentA.y, tangentA.x); // Fallback to normal of A

    avgTangent /= len;

    // Turn the average tangent into a normal vector (-y, x)
    sf::Vector2f miterNormal(-avgTangent.y, avgTangent.x);

    // Scale the miter width based on the angle to prevent squeezing/pinching
    const sf::Vector2f normalA(-tangentA.y, tangentA.x);
    const float dot = miterNormal.x * normalA.x + miterNormal.y * normalA.y;

    if (dot != 0.0f) {
        miterNormal /= dot; // Scale factor: 1 / cos(theta)
    }

    return miterNormal;
}


void RoadBuilder::buildSegment(Road& road) {
    // Configurations
    std::vector<LaneConfig> laneConfig = {
        {3.4}
    };
    std::vector<MarkingConfig> markingConfig = {
        {-1.5, SOLID}, {1.5, SOLID}
    };

    // Points
    const SnapPoint& p0 = points[0];
    const SnapPoint& p2 = (points.size() == 2) ? points[1] : points[2];
    const sf::Vector2f p1 = (points.size() == 2) ? (p0.position + p2.position) / 2.0f : points[1].position;

    auto segment = std::make_unique<Segment>(
        p0.position, p2.position, p1,
        laneConfig, markingConfig
    );

    if (p0.snapped && p0.targetSegment != nullptr) {
        connect(
            p0.targetSegment, true,
            segment.get(), false,
            laneConfig, markingConfig
        );
    }
    if (p2.snapped && p2.targetSegment != nullptr) {
        connect(
            p2.targetSegment, false,
            segment.get(), true,
            laneConfig, markingConfig
        );
    }

    points.clear();

    currentMousePos.snapped = true;
    currentMousePos.position = p2.position;
    currentMousePos.targetSegment = segment.get();

    points.push_back(currentMousePos);

    // Only transfer owner ship by the end of function
    road.addSegment(std::move(segment));
}


void RoadBuilder::setMousePosition(const sf::Vector2f pos, const Road& road) {
    const auto point = findSnapTarget(road, pos);

    if (point.snapped) {
        currentMousePos = point;
    } else {
        currentMousePos = {false, pos};
    }
}


void RoadBuilder::renderPreview(sf::RenderWindow* window) const {
    if (points.empty()) return;

    // Clear previous frame data without freeing the underlying capacity
    m_previewMesh.clear();
    m_leftOutline.clear();
    m_rightOutline.clear();

    const sf::Color fillCc(255, 255, 255, 60);
    const sf::Color outlineColor(255, 255, 255, 150);

    // Resolve control points
    const sf::Vector2f p0 = points[0].position;
    const sf::Vector2f p1 = (mode == STRAIGHT) ? (p0 + currentMousePos.position) / 2.0f
                    : (points.size() == 1 ? currentMousePos.position : points[1].position);
    const sf::Vector2f p2 = currentMousePos.position;

    // Match physical width mapping
    const float halfWidthPx = (3.4f * Scale::PPM) / 2.0f;

    sf::Vector2f prevLeftPos;
    sf::Vector2f prevRightPos;

    // Single mathematical pass for meshes and outlines
    for (int i = 0; i <= CURVEPOINTS; ++i) {
        float t = static_cast<float>(i) / CURVEPOINTS;

        // Inline Bezier point/normal logic to maximize cache locality
        const float u = 1.0f - t;
        sf::Vector2f center = u * u * p0 + 2.0f * u * t * p1 + t * t * p2;

        sf::Vector2f tangent = 2.0f * (1.0f - t) * (p1 - p0) + 2.0f * t * (p2 - p1);
        const float length = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
        sf::Vector2f normal = (length != 0.0f) ? sf::Vector2f(-tangent.y / length, tangent.x / length)
                                               : sf::Vector2f(0.0f, 0.0f);

        sf::Vector2f leftPos  = center - normal * halfWidthPx;
        sf::Vector2f rightPos = center + normal * halfWidthPx;

        // Construct triangles directly for the fill mesh
        if (i > 0) {
            m_previewMesh.append(sf::Vertex(prevLeftPos, fillCc));
            m_previewMesh.append(sf::Vertex(prevRightPos, fillCc));
            m_previewMesh.append(sf::Vertex(leftPos, fillCc));

            m_previewMesh.append(sf::Vertex(prevRightPos, fillCc));
            m_previewMesh.append(sf::Vertex(rightPos, fillCc));
            m_previewMesh.append(sf::Vertex(leftPos, fillCc));
        }

        // Populate outline strip points
        m_leftOutline.append(sf::Vertex(leftPos, outlineColor));
        m_rightOutline.append(sf::Vertex(rightPos, outlineColor));

        prevLeftPos = leftPos;
        prevRightPos = rightPos;
    }

    if (points[0].snapped) {
        sf::CircleShape snapIndicator(5);
        snapIndicator.setFillColor(sf::Color::White);
        snapIndicator.setPosition(points[0].position.x - 5, points[0].position.y - 5);
        window->draw(snapIndicator);
    }
    if (currentMousePos.snapped) {
        sf::CircleShape snapIndicator(5);
        snapIndicator.setFillColor(sf::Color::White);
        snapIndicator.setPosition(currentMousePos.position.x - 5, currentMousePos.position.y - 5);
        window->draw(snapIndicator);
    }

    window->draw(m_previewMesh);
    window->draw(m_leftOutline);
    window->draw(m_rightOutline);
}


SnapPoint RoadBuilder::findSnapTarget(const Road& road, const sf::Vector2f mousePos) {
    float closestDist = SNAPPING_DISTANCE;

    SnapPoint snap;

    for (const auto& segment : road.getSegments()) {
        const sf::Vector2f startPos = segment->getStart();
        const float distToStart = std::sqrt(std::pow(mousePos.x - startPos.x, 2) + std::pow(mousePos.y - startPos.y, 2));

        if (distToStart < closestDist) {
            closestDist = distToStart;
            snap.snapped = true;
            snap.position = startPos;
            snap.targetSegment = segment.get();

            // Tangent at t=0 is 2*(p1 - p0). Normalized:
            // sf::Vector2f tangent = segment->getCurvePoint() - startPos;
            // float len = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
            // snap.tangent = (len != 0.0f) ? tangent / len : sf::Vector2f(0.f, 0.f);
        }

        const sf::Vector2f endPos = segment->getEnd();
        const float distToEnd = std::sqrt(std::pow(mousePos.x - endPos.x, 2) + std::pow(mousePos.y - endPos.y, 2));

        if (distToEnd < closestDist) {
            closestDist = distToEnd;
            snap.snapped = true;
            snap.position = endPos;
            snap.targetSegment = segment.get();

            // Tangent at t=1 is 2*(p2 - p1). Normalized:
            // sf::Vector2f tangent = endPos - segment->getCurvePoint();
            // float len = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
            // snap.tangent = (len != 0.0f) ? tangent / len : sf::Vector2f(0.f, 0.f);
        }
    }

    return snap;
}


void RoadBuilder::connect(Segment* segmentA, const bool segmentAend, Segment* segmentB, const bool segmentBend, const std::vector<LaneConfig>& lanes, const std::vector<MarkingConfig>& markings) {
    if (segmentA == segmentB) return;

    sf::Vector2f tangentA = segmentAend ? (segmentA->getEnd() - segmentA->getCurvePoint())
                                      : (segmentA->getStart() - segmentA->getCurvePoint());
    float lenA = std::hypot(tangentA.x, tangentA.y);

    if (lenA != 0.0f) tangentA /= lenA;
    if (!segmentAend) tangentA = -tangentA;

    sf::Vector2f tangentB = segmentBend ? (segmentB->getCurvePoint() - segmentB->getEnd())
                                      : (segmentB->getCurvePoint() - segmentB->getStart());
    const float lenB = std::hypot(tangentB.x, tangentB.y);

    if (lenB != 0.0f) tangentB /= lenB;
    if (segmentBend) tangentB = -tangentB;

    const sf::Vector2f miter = calculateMiterNormal(tangentA, tangentB);

    const std::optional<sf::Vector2f> startA = segmentAend ? segmentA->getCustomStartNormal() : miter;
    const std::optional<sf::Vector2f> endA   = segmentAend ? miter : segmentA->getCustomEndNormal();
    segmentA->updateNormals(startA, endA, lanes, markings);

    const std::optional<sf::Vector2f> startB = segmentBend ? segmentB->getCustomStartNormal() : miter;
    const std::optional<sf::Vector2f> endB   = segmentBend ? miter : segmentB->getCustomEndNormal();
    segmentB->updateNormals(startB, endB, lanes, markings);
}
