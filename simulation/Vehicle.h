#ifndef TRAFFIX_VEHICLE_H
#define TRAFFIX_VEHICLE_H


#include <SFML/Graphics.hpp>


class Segment;

class Vehicle final : public sf::Drawable {
public:
    explicit Vehicle(Segment* segment);

    [[nodiscard]] Segment* getSegment() const { return segment; }

    void update(float deltaTime);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Sprite sprite;

    float speed = 10;
    float distance = 0;

    Segment* segment;
};


#endif //TRAFFIX_VEHICLE_H