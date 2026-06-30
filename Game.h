#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics.hpp"

#include "rendering/GameRenderer.h"


enum class GameState {
    Default, RoadBuildMenu, BuildingRoad
};


class GameRenderer;

class Game {
public:
    explicit Game(const std::shared_ptr<sf::RenderWindow>& window);

    void setState(GameState state);
    GameState getState();

    void update();
    void render();

private:
    std::shared_ptr<sf::RenderWindow> window;
    GameState state = GameState::Default;

    GameRenderer* gameRenderer;

    void handleEvents();
};



#endif //GAME_H
