//
// Created by kipteam on 4/6/25.
//

#ifndef GAME_H
#define GAME_H

#include "GameRenderer.h"


class Game {
public:
    explicit Game(sf::RenderWindow* window, sf::View* view);

    void update();
    void render();

private:
    GameRenderer* gameRenderer;

    sf::RenderWindow* window;
    sf::View* view;
};



#endif //GAME_H
