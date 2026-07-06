#ifndef TRAFFIX_SIMULATION_H
#define TRAFFIX_SIMULATION_H
#include <memory>
#include <vector>

#include "Vehicle.h"


class Simulation final : public sf::Drawable {
public:
    Simulation() = default;

    void update(double deltaTime);
    void addVehicle(Segment* segment);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<std::unique_ptr<Vehicle>> vehicles;
};


#endif //TRAFFIX_SIMULATION_H