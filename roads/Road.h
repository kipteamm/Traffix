#ifndef TRAFFIX_ROAD_H
#define TRAFFIX_ROAD_H

#include <memory>
#include <vector>

#include "SFML/Graphics.hpp"
#include "Segment.h"


constexpr int CURVEPOINTS = 20;


class Road final : public sf::Drawable {
public:
    Road() = default;

    void addSegment(std::unique_ptr<Segment> segment);
    [[nodiscard]] Segment* getLastSegment() const;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<std::unique_ptr<Segment>> segments;
};


#endif //TRAFFIX_ROAD_H