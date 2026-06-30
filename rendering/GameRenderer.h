//
// Created by kipteam on 4/6/25.
//

#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include "gui/UIManager.h"
#include "../Game.h"


class Game;
class UIManager;


class GameRenderer {
public:
    explicit GameRenderer(Game* game, const std::shared_ptr<sf::RenderWindow>& window);

    [[nodiscard]] sf::View& getCamera() { return camera; }

    void render();

    void resizeEvent(const sf::Event& event);
    [[nodiscard]] bool mouseClickEvent(const sf::Event& event);
    void mouseReleaseEvent(const sf::Event& event);
    void mouseWheelEvent(const sf::Event& event);

private:
    Game* game;

    std::shared_ptr<sf::RenderWindow> window;
    sf::View camera;
    sf::View ui;

    float zoom = 1.f;
    bool dragging = false;
    sf::Vector2i dragStart;

    UIManager* uiManager;

    void renderWorld();
    void handleMouseDrag();
};



#endif //GAMERENDERER_H
