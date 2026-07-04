#include "../rendering/AssetManager.h"
#include "Road.h"


void Road::addSegment(std::unique_ptr<Segment> segment) {
    segments.push_back(std::move(segment));
}


Segment* Road::getLastSegment() const {
    if (segments.empty()) return nullptr;
    return segments.back().get();
}


void Road::draw(sf::RenderTarget& target, sf::RenderStates states) const {
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

    if (visibleAsphalt.getVertexCount() == 0) return;

    states.texture = &AssetManager::getInstance().getRoadTexture();
    target.draw(visibleAsphalt, states);
    target.draw(visibleMarkings, states);
}
