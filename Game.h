//
// Created by kipteam on 4/6/25.
//

#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics.hpp"

#include "GameRenderer.h"


enum class GameState {Default, RoadBuildMenu, BuildingRoad};


class GameRenderer;

class Game {
public:
    explicit Game(sf::RenderWindow* window);

    void update();
    void render();

    void setState(GameState state);
    GameState getState();

private:
    GameRenderer* gameRenderer;
    GameState state = GameState::Default;
};



#endif //GAME_H
