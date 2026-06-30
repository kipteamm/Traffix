#include "Road.h"


void Road::addSegment(std::unique_ptr<Segment> segment) {
    segments.push_back(std::move(segment));
}


void Road::render(sf::RenderWindow* window) {
    for (auto& segment : segments) {
        if (!segment->isCurved()) {
            const sf::Vertex line[] = {
                sf::Vertex(segment->getStart(), sf::Color::White),
                sf::Vertex(segment->getEnd(), sf::Color::White)
            };
            window->draw(line, 2, sf::Lines);
            continue;
        }

        sf::VertexArray curve(sf::LineStrip, CURVEPOINTS);

        sf::Vector2f p0 = segment->getStart();
        sf::Vector2f p1 = segment->getCurve();
        sf::Vector2f p2 = segment->getEnd();

        for (int i = 0; i < CURVEPOINTS; ++i) {
            // t goes from 0.0 to 1.0
            const float t = static_cast<float>(i) / (CURVEPOINTS - 1);

            const float u = 1.f - t;
            const sf::Vector2f point = (u * u * p0) + (2.f * u * t * p1) + (t * t * p2);

            curve[i].position = point;
            curve[i].color = sf::Color::White;
        }

        window->draw(curve);
    }
}
