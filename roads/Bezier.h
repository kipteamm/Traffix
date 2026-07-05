#ifndef TRAFFIX_BEZIER_H
#define TRAFFIX_BEZIER_H


#include <SFML/System/Vector2.hpp>
#include <cmath>

inline sf::Vector2f getBezierPoint(const sf::Vector2f p0, const sf::Vector2f p1, const sf::Vector2f p2, const float t) {
    const float u = 1.0f - t;
    return u * u * p0 + 2.0f * u * t * p1 + t * t * p2;
}


inline sf::Vector2f getBezierNormal(const sf::Vector2f p0, const sf::Vector2f p1, const sf::Vector2f p2, const float t) {
    sf::Vector2f tangent = 2.0f * (1.0f - t) * (p1 - p0) + 2.0f * t * (p2 - p1);
    const float length = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
    if (length != 0) {
        tangent /= length;
    }
    return sf::Vector2f(-tangent.y, tangent.x);
}

#endif //TRAFFIX_BEZIER_H