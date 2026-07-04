#ifndef TRAFFIX_SEGMENT_H
#define TRAFFIX_SEGMENT_H

#include <SFML/Graphics.hpp>
#include <optional>


namespace Scale {
    constexpr float PPM = 32.0f;
    constexpr float LANE_WIDTH_METERS = 3.5f;
    constexpr float LANE_WIDTH_PX = LANE_WIDTH_METERS * PPM;
}


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


class Segment {
public:
    explicit Segment(sf::Vector2f start, sf::Vector2f end, sf::Vector2f curvePoint,
        const std::vector<LaneConfig>& lanes,
        const std::vector<MarkingConfig>& markings
    );

    [[nodiscard]] const sf::Vector2f& getStart() const { return start; }
    [[nodiscard]] const sf::Vector2f& getEnd() const { return end; }
    [[nodiscard]] const sf::Vector2f& getCurvePoint() const { return curvePoint; }

    [[nodiscard]] const std::optional<sf::Vector2f>& getCustomStartNormal() const { return customStartNormal; }
    [[nodiscard]] const std::optional<sf::Vector2f>& getCustomEndNormal() const { return customEndNormal; }

    [[nodiscard]] const sf::VertexArray& getAsphaltMesh() const { return asphaltMesh; }
    [[nodiscard]] const sf::VertexArray& getMarkingsMesh() const { return markingsMesh; }
    [[nodiscard]] sf::FloatRect getBounds() const { return asphaltMesh.getBounds(); }

    void updateNormals(
        std::optional<sf::Vector2f> startNormal,
        std::optional<sf::Vector2f> endNormal,
        const std::vector<LaneConfig>& lanes,
        const std::vector<MarkingConfig>& markings
    );

private:
    sf::Vector2f start;
    sf::Vector2f end;
    sf::Vector2f curvePoint;

    std::optional<sf::Vector2f> customStartNormal;
    std::optional<sf::Vector2f> customEndNormal;

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