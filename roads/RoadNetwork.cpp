#include "RoadNetwork.h"

#include <cmath>

#include "../rendering/AssetManager.h"


sf::Vector2f calculateMiterNormal(const sf::Vector2f tangentA, const sf::Vector2f tangentB) {
    // Get the average tangent vector
    sf::Vector2f avgTangent = tangentA + tangentB;

    // Handle perfectly flat/collinear or 180-degree turn edge-cases safely
    const float len = std::sqrt(avgTangent.x * avgTangent.x + avgTangent.y * avgTangent.y);
    if (len == 0.0f)
        return sf::Vector2f(-tangentA.y, tangentA.x); // Fallback to normal of A

    avgTangent /= len;

    // Turn the average tangent into a normal vector (-y, x)
    sf::Vector2f miterNormal(-avgTangent.y, avgTangent.x);

    // Scale the miter width based on the angle to prevent squeezing/pinching
    const sf::Vector2f normalA(-tangentA.y, tangentA.x);
    const float dot = miterNormal.x * normalA.x + miterNormal.y * normalA.y;

    if (dot != 0.0f) {
        miterNormal /= dot; // Scale factor: 1 / cos(theta)
    }

    return miterNormal;
}


void connect(Segment* segmentA, Segment* segmentB, const Node* commonNode) {
    if (segmentA == segmentB) return;

    const bool segmentAend = (segmentA->getEnd() == commonNode);
    const bool segmentBend = (segmentB->getEnd() == commonNode);

    // Calculate natural forward tangent for A
    sf::Vector2f tangentA = segmentAend ? (segmentA->getEnd()->position - segmentA->getCurvePoint())
                                        : (segmentA->getStart()->position - segmentA->getCurvePoint());
    float lenA = std::hypot(tangentA.x, tangentA.y);
    if (lenA != 0.0f) tangentA /= lenA;
    if (!segmentAend) tangentA = -tangentA;

    // Calculate natural forward tangent for B
    sf::Vector2f tangentB = segmentBend ? (segmentB->getCurvePoint() - segmentB->getEnd()->position)
                                        : (segmentB->getCurvePoint() - segmentB->getStart()->position);
    const float lenB = std::hypot(tangentB.x, tangentB.y);
    if (lenB != 0.0f) tangentB /= lenB;
    if (segmentBend) tangentB = -tangentB;

    // NEW LOGIC: Determine if the segments connect head-to-head or tail-to-tail
    const bool opposing = (segmentAend == segmentBend);

    // Force B's tangent to logically flow OUT of the node relative to A to get a proper bisector
    const sf::Vector2f flowB = opposing ? -tangentB : tangentB;
    const sf::Vector2f miter = calculateMiterNormal(tangentA, flowB);

    // Apply the miter. If flows oppose, B needs the inverted miter to match its local coordinate space.
    const sf::Vector2f miterA = miter;
    const sf::Vector2f miterB = opposing ? -miter : miter;

    const std::optional<sf::Vector2f> startA = segmentAend ? segmentA->getCustomStartNormal() : miterA;
    const std::optional<sf::Vector2f> endA   = segmentAend ? miterA : segmentA->getCustomEndNormal();
    segmentA->updateNormals(startA, endA);

    const std::optional<sf::Vector2f> startB = segmentBend ? segmentB->getCustomStartNormal() : miterB;
    const std::optional<sf::Vector2f> endB   = segmentBend ? miterB : segmentB->getCustomEndNormal();
    segmentB->updateNormals(startB, endB);
}


Node::Node(const sf::Vector2f position) : position(position) {}


void Node::addConnection(Segment* segment, RoadNetwork* network) {
    segments.push_back(segment);

    // Early return if this is the only segment
    if (segments.size() == 1) return;

    // Seamlessly connect segments.
    if (segments.size() == 2)
        return connect(segments[0], segments[1], this);

    // Node already is an intersection, update mesh
    if (isIntersection)
        return network->getIntersection(this)->buildMesh();

    // Node wasn't an intersection, but is one now
    isIntersection = true;
    network->createIntersection(this)->buildMesh();
}


Node* RoadNetwork::createNode(sf::Vector2f position) {
    auto node = std::make_unique<Node>(position);
    nodes.push_back(std::move(node));

    Node* ptr = nodes.back().get();

    const GridKey key = getGridKey(position);
    nodeGrid[key].push_back(ptr);

    return ptr;
}

Segment* RoadNetwork::createSegment(Node* start, Node* end, sf::Vector2f control, const RoadConfig& config) {
    auto segment = std::make_unique<Segment>(start, end, control, config);
    segments.push_back(std::move(segment));

    Segment* ptr = segments.back().get();

    // If this road is connected to just the one other road, continue the
    // texture as if it were one.
    if (start->segments.size() == 1) {
        float offset = 0.f;
        for (const Segment* seg: start->segments) {
            if (seg->getEnd() != start) continue;
            offset = seg->getVOffset() + seg->getLength();
        }

        ptr->setVOffset(offset);
    }

    start->addConnection(ptr, this);
    end->addConnection(ptr, this);

    // not sure how to do segment looking up

    return ptr;
}


Intersection* RoadNetwork::createIntersection(Node* node) {
    auto intersection = std::make_unique<Intersection>(node);
    Intersection* ptr = intersection.get();

    intersections[node] = std::move(intersection);

    return ptr;
}


GridKey RoadNetwork::getGridKey(const sf::Vector2f& pos) const {
    return {
        static_cast<int>(std::floor(pos.x / CELL_SIZE)),
        static_cast<int>(std::floor(pos.y / CELL_SIZE))
    };
}


Node* RoadNetwork::findNearestNode(const sf::Vector2f& position) {
    // Uses squares distances to avoid roots.

    const GridKey centerKey = getGridKey(position);

    Node* closestNode = nullptr;
    float closestDist = SNAPPING_DISTANCE * SNAPPING_DISTANCE;

    for (int offsetX = -1; offsetX <= 1; ++offsetX) {
        for (int offsetY = -1; offsetY <= 1; ++offsetY) {
            GridKey searchKey = { centerKey.x + offsetX, centerKey.y + offsetY };

            auto it = nodeGrid.find(searchKey);
            if (it == nodeGrid.end()) continue;

            for (Node* node : it->second) {
                const sf::Vector2f diff = node->position - position;
                const float dist = diff.x * diff.x + diff.y * diff.y;

                if (dist >= closestDist) continue;;

                closestDist = dist;
                closestNode = node;
            }
        }
    }

    return closestNode;
}


Intersection* RoadNetwork::getIntersection(const Node* node) const {
    const auto it = intersections.find(node);
    if (it == intersections.end()) return nullptr;

    return it->second.get();
}



void RoadNetwork::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    const sf::View currentView = target.getView();
    const sf::FloatRect viewBounds(
        currentView.getCenter() - currentView.getSize() / 2.f,
        currentView.getSize()
    );

    sf::VertexArray visibleAsphalt(sf::Triangles);
    sf::VertexArray visibleMarkings(sf::Triangles);

    for (const auto& segment : segments) {
        if (!viewBounds.intersects(segment->getBounds())) continue;

        // Copy asphalt mesh
        const auto& asphalt = segment->getAsphaltMesh();
        for (size_t i = 0; i < asphalt.getVertexCount(); ++i) {
            visibleAsphalt.append(asphalt[i]);
        }

        // Copy markingsmesh
        const auto& markings = segment->getMarkingsMesh();
        for (size_t i = 0; i < markings.getVertexCount(); ++i) {
            visibleMarkings.append(markings[i]);
        }
    }

    for (const auto& [_, intersection] : intersections) {
        const auto& asphalt = intersection->getAsphaltMesh();
        for (size_t i = 0; i < asphalt.getVertexCount(); ++i) {
            visibleAsphalt.append(asphalt[i]);
        }
    }

    if (visibleAsphalt.getVertexCount() == 0) return;

    states.texture = &AssetManager::getInstance().getRoadTexture();
    target.draw(visibleAsphalt, states);
    target.draw(visibleMarkings, states);

    // Draw nodes (intersections)
    // for (const auto& node : nodes) {
    //     if (!viewBounds.contains(node->position)) continue;
    //
    //     target.draw(*node, states);
    // }
}
