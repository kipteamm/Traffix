#ifndef TRAFFIX_ROADNETWORK_H
#define TRAFFIX_ROADNETWORK_H

#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

#include "Intersection.h"
#include "Segment.h"


struct GridKey {
    int x, y;

    bool operator==(const GridKey& other) const {
        return x == other.x && y == other.y;
    }
};

// Custom hash function for unordered_map
struct GridKeyHash {
    std::size_t operator()(const GridKey& k) const {
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    }
};


class RoadNetwork;

struct Node {
    sf::Vector2f position;
    bool isIntersection = false;

    std::vector<Segment*> segments;

    explicit Node(sf::Vector2f position);

    void addConnection(Segment* segment, RoadNetwork* network);
};


// Snapping radius in pixels
constexpr float SNAPPING_DISTANCE = 30.f;
constexpr float CELL_SIZE = SNAPPING_DISTANCE * 3;


class RoadNetwork final : public sf::Drawable {
public:
    RoadNetwork() = default;

    [[nodiscard]] Node* createNode(sf::Vector2f position);
    Segment* createSegment(Node* start, Node* end, sf::Vector2f control, const RoadConfig& config);
    [[nodiscard]] Intersection* createIntersection(Node* node);

    // temporary
    [[nodiscard]] Segment* getStart() const { return start; }

    [[nodiscard]] GridKey getGridKey(const sf::Vector2f& pos) const;

    [[nodiscard]] Node* findNearestNode(const sf::Vector2f& position);
    // Segment* findNearestSegment(const sf::Vector2f& position);
    [[nodiscard]] Intersection* getIntersection(const Node* node) const;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    // temporary:
    Segment* start = nullptr;

    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Segment>> segments;

    std::unordered_map<GridKey, std::vector<Node*>, GridKeyHash> nodeGrid;
    std::unordered_map<GridKey, std::vector<Segment*>, GridKeyHash> segmentGrid;
    std::unordered_map<const Node*, std::unique_ptr<Intersection>> intersections;
};


#endif //TRAFFIX_ROADNETWORK_H