#include "../rendering/AssetManager.h"
#include "../roads/RoadNetwork.h"
#include "Vehicle.h"

#include <cmath>


constexpr float CAR_WIDTH_PXS = 24.0;
constexpr float CAR_HEIGHT_PXS = 40.0;


Vehicle::Vehicle(Segment* segment) : segment(segment) {
    sprite.setTexture(AssetManager::getInstance().getVehicleTexture());
    sprite.setTextureRect(sf::IntRect(0, 0, CAR_WIDTH_PXS, CAR_HEIGHT_PXS));

    // Set origin to the center so rotation looks natural
    sprite.setOrigin(CAR_HEIGHT_PXS / 2, CAR_HEIGHT_PXS / 2);

    // Car dimensions -> 2m x 4m
    const float widthPx = 2.0f * Scale::PPM;
    const float lengthPx = 4.0f * Scale::PPM;

    const float scaleX = widthPx / CAR_WIDTH_PXS;
    const float scaleY = lengthPx / CAR_HEIGHT_PXS;

    sprite.setScale(scaleX, scaleY);
}


void Vehicle::update(const float deltaTime) {
    distance += speed * deltaTime;
    // std::cout << distance << std::endl;

    // temporary: stop at end of segment
    if (distance > segment->getLength()) {
        distance = segment->getLength();
    }

    const float t = segment->getT(distance);

    const sf::Vector2f p0 = segment->getStart()->position;
    const sf::Vector2f p1 = segment->getCurvePoint();
    const sf::Vector2f p2 = segment->getEnd()->position;

    const float u = 1.0f - t;
    const sf::Vector2f position = (u * u * p0) + (2.0f * u * t * p1) + (t * t * p2);
    sprite.setPosition(position);

    const sf::Vector2f tangent = 2.0f * u * (p1 - p0) + 2.0f * t * (p2 - p1);

    const float angleRad = std::atan2(tangent.y, tangent.x);
    const float angleDeg = angleRad * (180.0f / 3.14159265f);

    // SFML 0 degrees is right, cars face upwards: add 90 degrees to align it
    // with the tangent
    sprite.setRotation(angleDeg + 90.0f);
}


void Vehicle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite, states);
}
