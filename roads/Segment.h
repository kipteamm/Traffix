#ifndef TRAFFIX_SEGMENT_H
#define TRAFFIX_SEGMENT_H

#include "RoadConfig.h"
#include <SFML/Graphics.hpp>
#include <optional>


namespace Scale {
    constexpr float PPM = 32.0f;
    constexpr float LANE_WIDTH_METERS = 3.5f;
    constexpr float LANE_WIDTH_PX = LANE_WIDTH_METERS * PPM;
}


struct Node;


class Segment {
public:
    explicit Segment(Node* start, Node* end, sf::Vector2f curvePoint,
        const RoadConfig& config
    );

    [[nodiscard]] Node* getStart() const { return start; }
    [[nodiscard]] Node* getEnd() const { return end; }
    [[nodiscard]] const sf::Vector2f& getCurvePoint() const { return curvePoint; }

    void setVOffset(float offset);
    [[nodiscard]] float getVOffset() const { return m_vOffset; };
    [[nodiscard]] float getLength() const;

    [[nodiscard]] const std::optional<sf::Vector2f>& getCustomStartNormal() const { return customStartNormal; }
    [[nodiscard]] const std::optional<sf::Vector2f>& getCustomEndNormal() const { return customEndNormal; }

    [[nodiscard]] const sf::VertexArray& getAsphaltMesh() const { return asphaltMesh; }
    [[nodiscard]] const sf::VertexArray& getMarkingsMesh() const { return markingsMesh; }
    [[nodiscard]] sf::FloatRect getBounds() const { return asphaltMesh.getBounds(); }

    void updateNormals(
        std::optional<sf::Vector2f> startNormal,
        std::optional<sf::Vector2f> endNormal
    );

private:
    // Config
    Node* start;
    Node* end;
    sf::Vector2f curvePoint;
    RoadConfig config;

    // Mesh
    std::optional<sf::Vector2f> customStartNormal;
    std::optional<sf::Vector2f> customEndNormal;

    sf::VertexArray asphaltMesh;
    sf::VertexArray markingsMesh;

    float m_vOffset = 0.0f;
    std::vector<float> mDistances;

    void precalculateDistances();

    void generateAsphaltMesh();
    void generateMarkingsMesh();

    sf::Vector2f getBezierPoint(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, float t) const;
    sf::Vector2f getBezierNormal(sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, float t) const;
};


#endif //TRAFFIX_SEGMENT_H