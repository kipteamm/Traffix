#include "RoadBuilder.h"

#include <cassert>
#include <iostream>
#include <memory>


RoadBuilder::RoadBuilder() {
    // At most 3 points (start, curve, end)
    points.reserve(3);
}


void RoadBuilder::addPoint(const sf::Vector2f point) {
    points.push_back(std::move(point));
}


void RoadBuilder::setMode(const RoadBuildMode mode) {
    this->mode = mode;
}



void RoadBuilder::buildSegment(Road& road) {
    std::unique_ptr<Segment> segment;

    if (points.size() == 2) {
        segment = std::make_unique<Segment>(points[0], points[1]);
    } else if (points.size() == 3) {
        segment = std::make_unique<Segment>(points[0], points[2], points[1]);
    } else {
        std::cerr << "Not enough points" << std::endl;
    }

    points.clear();

    road.addSegment(std::move(segment));
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
            const float t = static_cast<float>(i) / (30 - 1);
            const float u = 1.f - t;
            const sf::Vector2f point = (u * u * p0) + (2.f * u * t * p1) + (t * t * p2);

            curve[i].position = point;
            curve[i].color = previewColor;
        }

        window->draw(curve);
    }
}