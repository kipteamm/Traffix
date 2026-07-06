#include "Simulation.h"

#include "../roads/Segment.h"


void Simulation::update(const double deltaTime) {
    for (auto& vehicle : vehicles) {
        vehicle->update(deltaTime);
    }
}


void Simulation::addVehicle(Segment* segment) {
    auto vehicle = std::make_unique<Vehicle>(segment);
    this->vehicles.push_back(std::move(vehicle));
}



void Simulation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (auto& vehicle : vehicles) {
        if (!vehicle->getSegment()->getVisible()) continue;

        vehicle->draw(target, states);
    }
}

