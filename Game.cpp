#include "Game.h"


Game::Game(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {
    gameRenderer = std::make_unique<GameRenderer>(this, window);
    roadBuilder = std::make_unique<RoadBuilder>();
    roadNetwork = std::make_unique<RoadNetwork>();
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

            case sf::Event::MouseButtonPressed: {
                // Click should go through to next event handler
                bool transcending = this->gameRenderer->mouseClickEvent(event);
                if (!transcending) break;

                if (state == GameState::BuildingRoad) {
                    transcending = roadBuilder->mouseClickEvent(event, roadNetwork.get());
                    if (!transcending) break;
                }

                break;
            }

            case sf::Event::MouseButtonReleased:
                this->gameRenderer->mouseReleaseEvent(event);
                break;

            case sf::Event::MouseWheelScrolled:
                this->gameRenderer->mouseWheelEvent(event);
                break;

            case sf::Event::MouseMoved: {
                if (this->state != GameState::BuildingRoad) break;

                sf::Vector2f mousePos = window->mapPixelToCoords(
                    sf::Mouse::getPosition(*window), gameRenderer->getCamera()
                );

                roadBuilder->setMousePosition(mousePos, roadNetwork.get());
                break;
            }

            default: break;
        }
    }
}
