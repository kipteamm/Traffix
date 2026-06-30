#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics.hpp"

#include "rendering/GameRenderer.h"
#include "roads/RoadBuilder.h"


enum class GameState {
    Default, RoadBuildMenu, BuildingRoad
};


class GameRenderer;

class Game {
public:
    explicit Game(const std::shared_ptr<sf::RenderWindow>& window);

    void setState(GameState state);
    GameState getState();

    [[nodiscard]] RoadBuilder* getRoadBuilder() const { return roadBuilder.get(); }

    [[nodiscard]] Road* getRoad() const { return road; };

    void update();
    void render();

private:
    std::shared_ptr<sf::RenderWindow> window;
    GameState state = GameState::Default;

    std::unique_ptr<GameRenderer> gameRenderer;
    std::unique_ptr<RoadBuilder> roadBuilder;

    // TEMPORARY
    Road* road = new Road;

    void handleEvents();
};



#endif //GAME_H
