#ifndef TRAFFIX_ROADBUILDER_H
#define TRAFFIX_ROADBUILDER_H

#include <vector>
#include <SFML/System/Vector2.hpp>

#include "Road.h"


struct SnapPoint {
    bool snapped = false;
    sf::Vector2f position;
    Segment* targetSegment = nullptr;
};


enum RoadBuildMode {
    STRAIGHT, CURVED
};


class RoadBuilder {
public:
    explicit RoadBuilder();

    void addPoint();
    [[nodiscard]] int total() const { return points.size(); }
    [[nodiscard]] bool popPoint();

    void setMode(RoadBuildMode newMode);
    [[nodiscard]] RoadBuildMode getMode() const { return mode; }

    void buildSegment(Road& road);

    void setMousePosition(sf::Vector2f pos, const Road& road);
    void renderPreview(sf::RenderWindow* window) const;

private:
    std::vector<SnapPoint> points;
    RoadBuildMode mode = STRAIGHT;

    SnapPoint currentMousePos;

    mutable sf::VertexArray m_previewMesh{sf::Triangles};
    mutable sf::VertexArray m_leftOutline{sf::LineStrip};
    mutable sf::VertexArray m_rightOutline{sf::LineStrip};

    SnapPoint findSnapTarget(const Road& road, sf::Vector2f mousePos);
    void connect(
        Segment* segmentA, bool segmentAend,
        Segment* segmentB, bool segmentBend,
        const std::vector<LaneConfig>& lanes,
        const std::vector<MarkingConfig>& markings
    );
};


#endif //TRAFFIX_ROADBUILDER_H