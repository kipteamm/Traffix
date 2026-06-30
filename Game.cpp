//
// Created by kipteam on 4/6/25.
//

#include "Game.h"


Game::Game(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {
    gameRenderer = new GameRenderer(this, window);
}


void Game::render() {
    handleEvents();
    this->gameRenderer->render();
}


void Game::update() {
    return;
}


void Game::setState(const GameState state) {
    this->state = state;
}

GameState Game::getState() {
    return this->state;
}


void Game::handleEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;

            case sf::Event::Resized:
                this->gameRenderer->resizeEvent(event);
                break;

            case sf::Event::MouseButtonPressed:
                this->gameRenderer->mouseClickEvent(event);
                break;

            case sf::Event::MouseButtonReleased:
                this->gameRenderer->mouseReleaseEvent(event);
                break;

            case sf::Event::MouseWheelMoved:
                this->gameRenderer->mouseWheelEvent(event);
                break;

            default: break;
        }
    }
}
