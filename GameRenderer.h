//
// Created by kipteam on 4/6/25.
//

#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include "Game.h"


class Game;

class GameRenderer {
public:
    explicit GameRenderer(Game* game, sf::RenderWindow* window, sf::View* view);

private:
    Game* game;

    sf::RenderWindow* window;
    sf::View* view;
};



#endif //GAMERENDERER_H
