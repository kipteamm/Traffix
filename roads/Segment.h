#ifndef TRAFFIX_SEGMENT_H
#define TRAFFIX_SEGMENT_H

#include <SFML/Graphics.hpp>


class Segment final : public sf::Drawable {
public:
    explicit Segment(sf::Vector2f start, sf::Vector2f end, sf::Vector2f curvePoint = {0, 0});

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::VertexArray mesh;

    sf::Vector2f getBezierPoint(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, float t) const;
    sf::Vector2f getBezierNormal(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, float t) const;
};


#endif //TRAFFIX_SEGMENT_H