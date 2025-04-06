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

    void render();

private:
    Game* game;

    sf::RenderWindow* window;
    sf::View* camera;

    float zoom = 1.f;
    bool dragging = false;
    sf::Vector2i dragStart;

    void handleEvents();

    void windowViewEvent(const sf::Event& event) const;
    void mouseClickEvent(const sf::Event& event);
    void mouseReleaseEvent(const sf::Event& event);
    void mouseWheelEvent(const sf::Event& event);
    void handleMouseDrag();
};



#endif //GAMERENDERER_H
