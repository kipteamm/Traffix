#ifndef TRAFFIX_SEGMENT_H
#define TRAFFIX_SEGMENT_H

#include <SFML/System/Vector2.hpp>


class Segment {
public:
    explicit Segment(sf::Vector2f start, sf::Vector2f end);
    explicit Segment(sf::Vector2f start, sf::Vector2f end, sf::Vector2f curvePoint);

    [[nodiscard]] sf::Vector2f& getStart() { return start; }
    [[nodiscard]] sf::Vector2f& getEnd() { return end; }
    [[nodiscard]] sf::Vector2f& getCurve() { return curvePoint; }
    [[nodiscard]] bool isCurved() const { return curved; }

private:
    sf::Vector2f start;
    sf::Vector2f end;
    sf::Vector2f curvePoint;
    bool curved = false;
};


#endif //TRAFFIX_SEGMENT_H