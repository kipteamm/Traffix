#include "RoadBuilder.h"

#include <iostream>
#include <optional>
#include <memory>
#include <cmath>


RoadBuilder::RoadBuilder() {
    // At most 3 points (start, curve, end)
    points.reserve(3);
    config = CONFIGURATIONS["Single lane - One Directional"];
}


void RoadBuilder::setMode(const RoadBuildMode newMode) {
    this->mode = newMode;
}


void RoadBuilder::setConfig(const RoadConfig& newConfig) {
    this->config = newConfig;
}


bool RoadBuilder::mouseClickEvent(const sf::Event& event, RoadNetwork* network) {
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
        if (points.empty()) return true;
        points.pop_back();
        return false;
    }

    points.push_back(SnapPoint(currentMousePos.get()));

    // Early return unfinished segments
    if (mode == STRAIGHT && points.size() < 2) return false;
    if (mode == CURVED   && points.size() < 3) return false;

    buildSegment(network);

    return false;
}


void RoadBuilder::buildSegment(RoadNetwork* network) {
    // Points
    const SnapPoint& p0 = points[0];
    const SnapPoint& p2 = (points.size() == 2) ? points[1] : points[2];
    const sf::Vector2f p1 = (points.size() == 2) ? (p0.position + p2.position) / 2.0f : points[1].position;

    // Add new segment (and possible nodes)
    Node* startNode = p0.node;
    if (startNode == nullptr)
        startNode = network->createNode(p0.position);

    Node* endNode = p2.node;
    if (endNode == nullptr)
        endNode = network->createNode(p2.position);

    network->createSegment(startNode, endNode, p1, config);

    // Reset builder state
    points.clear();

    currentMousePos->snapped = true;
    currentMousePos->position = p2.position;
    currentMousePos->node = endNode;

    points.push_back(SnapPoint(currentMousePos.get()));
}


void RoadBuilder::setMousePosition(const sf::Vector2f pos, RoadNetwork* network) const {
    Node* snappedNode = network->findNearestNode(pos);

    if (snappedNode) {
        currentMousePos->snapped = true;
        currentMousePos->position = snappedNode->position;
        currentMousePos->node = snappedNode;
    } else {
        currentMousePos->snapped = false;
        currentMousePos->position = pos;
        currentMousePos->node = nullptr;
    }
}


void RoadBuilder::renderPreview(sf::RenderWindow* window) const {
    if (points.empty()) return;

    // Clear previous frame data without freeing the underlying capacity
    m_previewMesh.clear();
    m_leftOutline.clear();
    m_rightOutline.clear();

    const sf::Color fillCc(255, 255, 255, 60);
    const sf::Color outlineColor(255, 255, 255, 150);

    // Resolve control points
    const sf::Vector2f p0 = points[0].position;
    const sf::Vector2f p1 = (mode == STRAIGHT) ? (p0 + currentMousePos->position) / 2.0f
                    : (points.size() == 1 ? currentMousePos->position : points[1].position);
    const sf::Vector2f p2 = currentMousePos->position;

    // Match physical width mapping
    float width = 0;

    for (const auto& lane: config.lanes) {
        width += lane.widthMeters;
    }

    const float halfWidthPx = (width * Scale::PPM) / 2.0f;

    sf::Vector2f prevLeftPos;
    sf::Vector2f prevRightPos;

    // Single mathematical pass for meshes and outlines
    for (int i = 0; i <= CURVEPOINTS; ++i) {
        float t = static_cast<float>(i) / CURVEPOINTS;

        // Inline Bezier point/normal logic to maximize cache locality
        const float u = 1.0f - t;
        sf::Vector2f center = u * u * p0 + 2.0f * u * t * p1 + t * t * p2;

        sf::Vector2f tangent = 2.0f * (1.0f - t) * (p1 - p0) + 2.0f * t * (p2 - p1);
        const float length = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
        sf::Vector2f normal = (length != 0.0f) ? sf::Vector2f(-tangent.y / length, tangent.x / length)
                                               : sf::Vector2f(0.0f, 0.0f);

        sf::Vector2f leftPos  = center - normal * halfWidthPx;
        sf::Vector2f rightPos = center + normal * halfWidthPx;

        // Construct triangles directly for the fill mesh
        if (i > 0) {
            m_previewMesh.append(sf::Vertex(prevLeftPos, fillCc));
            m_previewMesh.append(sf::Vertex(prevRightPos, fillCc));
            m_previewMesh.append(sf::Vertex(leftPos, fillCc));

            m_previewMesh.append(sf::Vertex(prevRightPos, fillCc));
            m_previewMesh.append(sf::Vertex(rightPos, fillCc));
            m_previewMesh.append(sf::Vertex(leftPos, fillCc));
        }

        // Populate outline strip points
        m_leftOutline.append(sf::Vertex(leftPos, outlineColor));
        m_rightOutline.append(sf::Vertex(rightPos, outlineColor));

        prevLeftPos = leftPos;
        prevRightPos = rightPos;
    }

    if (points[0].snapped) {
        sf::CircleShape snapIndicator(5);
        snapIndicator.setFillColor(sf::Color::White);
        snapIndicator.setPosition(points[0].position.x - 5, points[0].position.y - 5);
        window->draw(snapIndicator);
    }
    if (currentMousePos->snapped) {
        sf::CircleShape snapIndicator(5);
        snapIndicator.setFillColor(sf::Color::White);
        snapIndicator.setPosition(currentMousePos->position.x - 5, currentMousePos->position.y - 5);
        window->draw(snapIndicator);
    }

    window->draw(m_previewMesh);
    window->draw(m_leftOutline);
    window->draw(m_rightOutline);
}
