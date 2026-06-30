#include "Segment.h"


Segment::Segment(const sf::Vector2f start, const sf::Vector2f end)
    : start(start), end(end), curvePoint({0, 0}) {}

Segment::Segment(const sf::Vector2f start, const sf::Vector2f end, const sf::Vector2f curvePoint)
    : start(start), end(end), curvePoint(curvePoint) {}
