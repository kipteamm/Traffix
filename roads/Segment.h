#ifndef TRAFFIX_SEGMENT_H
#define TRAFFIX_SEGMENT_H

#include <SFML/Graphics.hpp>


enum MarkingType {
    NONE, SOLID, DASHED
};


struct LaneConfig {
    float widthMeters;
};

struct MarkingConfig {
    // Where is it located relative to the road center?
    float offsetFromRoadCenterMeters;
    MarkingType markingType;
};


class Segment final : public sf::Drawable {
public:
    explicit Segment(sf::Vector2f start, sf::Vector2f end, sf::Vector2f curvePoint,
        const std::vector<LaneConfig>& lanes,
        const std::vector<MarkingConfig>& markings
    );

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Vector2f start;
    sf::Vector2f end;
    sf::Vector2f curvePoint;

    sf::VertexArray asphaltMesh;
    sf::VertexArray markingsMesh;

    std::vector<float> mDistances;

    void precalculateDistances();

    void generateAsphaltMesh(const std::vector<LaneConfig>& lanes);
    void generateMarkingsMesh(const std::vector<MarkingConfig>& markings);

    sf::Vector2f getBezierPoint(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, float t) const;
    sf::Vector2f getBezierNormal(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, float t) const;
};


#endif //TRAFFIX_SEGMENT_H