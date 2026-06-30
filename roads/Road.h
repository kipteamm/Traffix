#ifndef TRAFFIX_ROAD_H
#define TRAFFIX_ROAD_H

#include <memory>
#include <vector>

#include "SFML/Graphics.hpp"
#include "Segment.h"


constexpr int CURVEPOINTS = 20;


class Road {
public:
    Road() = default;

    void addSegment(std::unique_ptr<Segment> segment);

    void render(sf::RenderWindow* window);

private:
    std::vector<std::unique_ptr<Segment>> segments;
};


#endif //TRAFFIX_ROAD_H