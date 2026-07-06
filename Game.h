#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics.hpp"

#include "rendering/GameRenderer.h"
#include "roads/RoadBuilder.h"
#include "roads/RoadNetwork.h"
#include "simulation/Simulation.h"


enum class GameState {
    Default, RoadBuildMenu, BuildingRoad
};


class GameRenderer;


class Game {
public:
    explicit Game(const std::shared_ptr<sf::RenderWindow>& window);

    void setState(GameState state);
    GameState getState() const;

    [[nodiscard]] RoadBuilder* getRoadBuilder() const { return roadBuilder.get(); }
    [[nodiscard]] RoadNetwork* getRoadNetwork() const { return roadNetwork.get(); }
    [[nodiscard]] Simulation* getSimulation() const { return simulation.get(); }

    void update();
    void render();

private:
    // config
    int time = 0;
    double deltaT = 10.0 / 60.0;

    std::shared_ptr<sf::RenderWindow> window;
    GameState state = GameState::Default;

    std::unique_ptr<GameRenderer> gameRenderer;
    std::unique_ptr<RoadBuilder> roadBuilder;
    std::unique_ptr<RoadNetwork> roadNetwork;
    std::unique_ptr<Simulation> simulation;

    void handleEvents();
};



#endif //GAME_H
