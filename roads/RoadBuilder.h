#ifndef TRAFFIX_ROADBUILDER_H
#define TRAFFIX_ROADBUILDER_H

#include <vector>
#include <SFML/System/Vector2.hpp>

#include "RoadNetwork.h"


struct SnapPoint {
    bool snapped = false;
    sf::Vector2f position;
    Node* node = nullptr;

    explicit SnapPoint() = default;
    explicit SnapPoint(const SnapPoint* point)
        : snapped(point->snapped), position(point->position), node(point->node) {}
};


enum RoadBuildMode {
    STRAIGHT, CURVED
};


class RoadBuilder {
public:
    explicit RoadBuilder();

    void setMode(RoadBuildMode newMode);
    [[nodiscard]] RoadBuildMode getMode() const { return mode; }
    void setConfig(const RoadConfig& newConfig);

    bool mouseClickEvent(const sf::Event &event, RoadNetwork* network);

    void setMousePosition(sf::Vector2f pos, RoadNetwork* network) const;
    void renderPreview(sf::RenderWindow* window) const;

private:
    std::vector<SnapPoint> points;
    RoadBuildMode mode = STRAIGHT;
    RoadConfig config;

    std::unique_ptr<SnapPoint> currentMousePos = std::make_unique<SnapPoint>();

    mutable sf::VertexArray m_previewMesh{sf::Triangles};
    mutable sf::VertexArray m_leftOutline{sf::LineStrip};
    mutable sf::VertexArray m_rightOutline{sf::LineStrip};

    void buildSegment(RoadNetwork* network);
};


#endif //TRAFFIX_ROADBUILDER_H