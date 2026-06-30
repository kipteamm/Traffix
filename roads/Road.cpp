#include "Road.h"


void Road::addSegment(std::unique_ptr<Segment> segment) {
    segments.push_back(std::move(segment));
}


void Road::render(sf::RenderWindow* window) {
    for (auto& segment : segments) {
        const sf::Vertex line[] = {
            sf::Vertex(segment->getStart(), sf::Color::White),
            sf::Vertex(segment->getEnd(), sf::Color::White)
        };

        window->draw(line, 2, sf::Lines);
    }
}
