//
// Created by kipteam on 4/6/25.
//

#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include "UIManager.h"
#include "Game.h"


class Game;
class UIManager;


class GameRenderer {
public:
    explicit GameRenderer(Game* game, sf::RenderWindow* window);

    void render();

private:
    Game* game;

    sf::RenderWindow* window;
    sf::View camera;
    sf::View ui;

    float zoom = 1.f;
    bool dragging = false;
    sf::Vector2i dragStart;

    UIManager* uiManager;

    void renderWorld();

    void handleEvents();
    void windowViewEvent(const sf::Event& event);
    void mouseClickEvent(const sf::Event& event);
    void mouseReleaseEvent(const sf::Event& event);
    void mouseWheelEvent(const sf::Event& event);
    void handleMouseDrag();
};



#endif //GAMERENDERER_H
