//
// Created by kipteam on 4/6/25.
//

#include "Game.h"


Game::Game(sf::RenderWindow *window, sf::View *view) {
    gameRenderer = new GameRenderer(this, window, view);
}


void Game::render() {
    this->gameRenderer->render();
}

void Game::update() {
    return;
}
