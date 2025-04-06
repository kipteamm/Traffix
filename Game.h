//
// Created by kipteam on 4/6/25.
//

#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics.hpp"

#include "GameRenderer.h"


class GameRenderer;

class Game {
public:
    explicit Game(sf::RenderWindow* window, sf::View* view);

    void update();
    void render();

private:
    GameRenderer* gameRenderer;
};



#endif //GAME_H
