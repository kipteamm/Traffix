#ifndef TRAFFIX_ROADBUILDER_H
#define TRAFFIX_ROADBUILDER_H

#include <vector>
#include <SFML/System/Vector2.hpp>

#include "Segment.h"
#include "Road.h"


class RoadBuilder {
public:
    explicit RoadBuilder();

    void addPoint(sf::Vector2f point);
    [[ nodiscard]] int total() const { return points.size(); }

    void buildSegment(Road& road);

private:
    std::vector<sf::Vector2f> points;
};


#endif //TRAFFIX_ROADBUILDER_H