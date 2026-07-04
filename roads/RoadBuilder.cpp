#include "RoadBuilder.h"

#include <iostream>
#include <optional>
#include <memory>
#include <cmath>


RoadBuilder::RoadBuilder() {
    // At most 3 points (start, curve, end)
    points.reserve(3);
}


void RoadBuilder::addPoint(const sf::Vector2f point) {
    points.push_back(std::move(point));
}


bool RoadBuilder::popPoint() {
    if (points.empty()) return false;

    points.pop_back();
    return true;
}



void RoadBuilder::setMode(const RoadBuildMode mode) {
    this->mode = mode;
}


sf::Vector2f calculateMiterNormal(const sf::Vector2f tangentA, const sf::Vector2f tangentB) {
    // 1. Get the average tangent vector
    sf::Vector2f avgTangent = tangentA + tangentB;

    // Handle perfectly flat/collinear or 180-degree turn edge-cases safely
    const float len = std::sqrt(avgTangent.x * avgTangent.x + avgTangent.y * avgTangent.y);
    if (len == 0.0f)
        return sf::Vector2f(-tangentA.y, tangentA.x); // Fallback to normal of A

    avgTangent /= len;

    // 2. Turn the average tangent into a normal vector (-y, x)
    sf::Vector2f miterNormal(-avgTangent.y, avgTangent.x);

    // 3. Scale the miter width based on the angle to prevent squeezing/pinching
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
    const sf::Vector2f p0 = points[0];
    const sf::Vector2f p2 = (points.size() == 2) ? points[1] : points[2];
    const sf::Vector2f p1 = (points.size() == 2) ? (p0 + p2) / 2.0f : points[1];

    // Connectivity
    std::optional<sf::Vector2f> startNormal = std::nullopt;
    std::optional<sf::Vector2f> endNormal = std::nullopt;

    Segment* lastSegment = road.getLastSegment();
    if (lastSegment != nullptr) {
        // Calculate ending tangent of previous segment (at t = 1.0)
        // Derivative of Bezier: B'(t) = 2(1-t)(p1-p0) + 2t(p2-p1)
        // For t = 1.0, B'(1) = 2 * (p2 - p1)
        const sf::Vector2f lastP1 = lastSegment->getCurvePoint();
        const sf::Vector2f lastP2 = lastSegment->getEnd();
        sf::Vector2f tangentA = lastP2 - lastP1;

        const float lenA = std::sqrt(tangentA.x * tangentA.x + tangentA.y * tangentA.y);
        if (lenA != 0) tangentA /= lenA;

        // Calculate starting tangent of our new segment (at t = 0.0)
        // B'(0) = 2 * (p1 - p0)
        sf::Vector2f tangentB = p1 - p0;
        const float lenB = std::sqrt(tangentB.x * tangentB.x + tangentB.y * tangentB.y);
        if (lenB != 0) tangentB /= lenB;

        // Calculate unified joint normal vector
        sf::Vector2f miter = calculateMiterNormal(tangentA, tangentB);

        startNormal = miter;

        lastSegment->updateNormals(
            lastSegment->getCustomStartNormal(),
            miter,
            laneConfig,
            markingConfig
        );
    }


    auto segment = std::make_unique<Segment>(
        p0, p2, p1,
        laneConfig, markingConfig,
        startNormal, endNormal
    );

    road.addSegment(std::move(segment));

    points.clear();
    points.push_back(p2);
}


void RoadBuilder::setMousePosition(const sf::Vector2f pos) {
    currentMousePos = pos;
}


void RoadBuilder::renderPreview(sf::RenderWindow* window) const {
    if (points.size() < 1) return;

    const sf::Color previewColor(255, 255, 255, 128);

    if (mode == STRAIGHT && points.size() == 1) {
        const sf::Vertex line[] = {
            sf::Vertex(points[0], previewColor),
            sf::Vertex(currentMousePos, previewColor)
        };
        window->draw(line, 2, sf::Lines);

        return;
    }

    if (mode == CURVED) {
        const sf::Vector2f p0 = points[0];
        const sf::Vector2f p2 = currentMousePos;

        sf::Vector2f p1;
        if (points.size() == 1) {
            p1 = currentMousePos;
        } else if (points.size() == 2) {
            p1 = points[1];
        }

        sf::VertexArray curve(sf::LineStrip, CURVEPOINTS);

        for (int i = 0; i < CURVEPOINTS; ++i) {
            const float t = static_cast<float>(i) / (CURVEPOINTS - 1);
            const float u = 1.f - t;
            const sf::Vector2f point = (u * u * p0) + (2.f * u * t * p1) + (t * t * p2);

            curve[i].position = point;
            curve[i].color = previewColor;
        }

        window->draw(curve);
    }
}