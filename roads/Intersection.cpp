#include "Intersection.h"
#include "RoadNetwork.h"
#include "Bezier.h"


Intersection::Intersection(Node* centerNode) : centerNode(centerNode) {}


void Intersection::buildMesh() {
    asphaltMesh.clear();
    markingsMesh.clear();

    if (centerNode->segments.size() < 3) return;

    generateAsphaltMesh();
    generateMarkingsMesh();
}


bool intersectLines(const sf::Vector2f& p1, const sf::Vector2f& d1,
                    const sf::Vector2f& p2, const sf::Vector2f& d2,
                    sf::Vector2f& outIntersection) {
    const float cross = d1.x * d2.y - d1.y * d2.x;

    // FIX: Increased threshold to 0.01f. If roads are nearly parallel
    // (like the straight top of a T-junction), ignore the collision.
    if (std::abs(cross) < 0.01f) return false;

    const sf::Vector2f diff = p2 - p1;
    const float t = (diff.x * d2.y - diff.y * d2.x) / cross;

    outIntersection = p1 + d1 * t;
    return true;
}


void Intersection::generateAsphaltMesh() {
    if (centerNode->segments.size() < 3) return;

    struct SegmentData {
        Segment* segment;
        bool isStart;
        sf::Vector2f tangent;
        sf::Vector2f rightNormal;
        float halfWidth;
        float angle;
        float calculatedSetback = 0.0f;
    };

    std::vector<SegmentData> segData;
    const sf::Vector2f centerPos = centerNode->position;

    // Gather segment data
    for (Segment* seg : centerNode->segments) {
        const bool isStart = (seg->getStart() == centerNode);

        sf::Vector2f tangent = seg->getCurvePoint() - centerPos;
        const float len = std::hypot(tangent.x, tangent.y);
        if (len != 0.0f) tangent /= len;

        const sf::Vector2f rightNormal(-tangent.y, tangent.x);
        const float angle = std::atan2(tangent.y, tangent.x);
        const float halfWidth = (seg->getLaneWidth() * Scale::PPM) / 2.0f;

        segData.push_back({seg, isStart, tangent, rightNormal, halfWidth, angle, 0.0f});
    }

    // Sort data clockwise so the intersection can be cleanly generated
    std::sort(segData.begin(), segData.end(), [](const SegmentData& a, const SegmentData& b) {
        return a.angle < b.angle;
    });

    // Calculate Setbacks for segments
    const size_t numSegs = segData.size();
    for (size_t i = 0; i < numSegs; ++i) {
        const size_t nextIdx = (i + 1) % numSegs;
        SegmentData& current = segData[i];
        SegmentData& next = segData[nextIdx];

        sf::Vector2f currentRightEdge = centerPos + current.rightNormal * current.halfWidth;
        sf::Vector2f nextLeftEdge = centerPos - next.rightNormal * next.halfWidth;

        sf::Vector2f intersectionPoint;

        if (!intersectLines(currentRightEdge, current.tangent, nextLeftEdge, next.tangent, intersectionPoint)) continue;

        const sf::Vector2f toIntersection = intersectionPoint - centerPos;
        const float setbackCurrent = (toIntersection.x * current.tangent.x) + (toIntersection.y * current.tangent.y);
        const float setbackNext = (toIntersection.x * next.tangent.x) + (toIntersection.y * next.tangent.y);

        current.calculatedSetback = std::max(current.calculatedSetback, setbackCurrent);
        next.calculatedSetback = std::max(next.calculatedSetback, setbackNext);
    }

    // Generate intersection mesh
    std::vector<sf::Vertex> boundaryPoints;
    for (const SegmentData& data : segData) {
        if (data.isStart) {
            data.segment->setStartSetback(data.calculatedSetback);
            data.segment->updateNormals(std::nullopt, data.segment->getCustomEndNormal());
        } else {
            data.segment->setEndSetback(data.calculatedSetback);
            data.segment->updateNormals(data.segment->getCustomStartNormal(), std::nullopt);
        }

        const float targetDist = data.isStart
            ? data.calculatedSetback
            : (data.segment->getLength() - data.calculatedSetback);

        const float t = data.segment->getT(targetDist);

        sf::Vector2f cutoffCenter = getBezierPoint(data.segment->getStart()->position, data.segment->getCurvePoint(), data.segment->getEnd()->position, t);

        sf::Vector2f leftEdge = cutoffCenter - data.rightNormal * data.halfWidth;
        sf::Vector2f rightEdge = cutoffCenter + data.rightNormal * data.halfWidth;

        boundaryPoints.push_back(sf::Vertex(leftEdge, sf::Color(100, 100, 100)));
        boundaryPoints.push_back(sf::Vertex(rightEdge, sf::Color(100, 100, 100)));
    }

    for (size_t i = 0; i < boundaryPoints.size(); ++i) {
        const size_t nextIndex = (i + 1) % boundaryPoints.size();
        asphaltMesh.append(sf::Vertex(centerPos, sf::Color(100, 100, 100)));
        asphaltMesh.append(boundaryPoints[i]);
        asphaltMesh.append(boundaryPoints[nextIndex]);
    }
}


void Intersection::generateMarkingsMesh() {}
