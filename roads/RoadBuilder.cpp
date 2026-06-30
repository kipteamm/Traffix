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


void RoadBuilder::buildSegment(Road& road) {
    std::unique_ptr<Segment> segment;

    if (points.size() == 2) {
        segment = std::make_unique<Segment>(points[0], points[1]);
    } else if (points.size() == 3) {
        segment = std::make_unique<Segment>(points[0], points[1], points[2]);
    } else {
        std::cerr << "Not enough points" << std::endl;
    }

    points.clear();

    road.addSegment(std::move(segment));
}

