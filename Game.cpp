//
// Created by kipteam on 4/6/25.
//

#include "Game.h"


Game::Game(sf::RenderWindow *window) {
    gameRenderer = new GameRenderer(this, window);
}


void Game::render() {
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

