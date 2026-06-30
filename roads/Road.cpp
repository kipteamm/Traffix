#include "Road.h"


void Road::addSegment(std::unique_ptr<Segment> segment) {
    segments.push_back(std::move(segment));
}


void Road::render(sf::RenderWindow* window) const {
    for (const auto& segment : segments) {
        window->draw(*segment);
    }
}
