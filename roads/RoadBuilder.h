#ifndef TRAFFIX_ROADBUILDER_H
#define TRAFFIX_ROADBUILDER_H

#include <vector>
#include <SFML/System/Vector2.hpp>

#include "Road.h"


enum RoadBuildMode {
    STRAIGHT, CURVED
};


class RoadBuilder {
public:
    explicit RoadBuilder();

    void addPoint(sf::Vector2f point);
    [[nodiscard]] int total() const { return points.size(); }

    void setMode(RoadBuildMode mode);
    [[nodiscard]] RoadBuildMode getMode() const { return mode; }

    void buildSegment(Road& road);

    void setMousePosition(sf::Vector2f pos);
    void renderPreview(sf::RenderWindow* window) const;

private:
    std::vector<sf::Vector2f> points;
    RoadBuildMode mode = STRAIGHT;

    sf::Vector2f currentMousePos;
};


#endif //TRAFFIX_ROADBUILDER_H