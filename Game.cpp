#include "Game.h"


Game::Game(const std::shared_ptr<sf::RenderWindow>& window) : window(window) {
    gameRenderer = std::make_unique<GameRenderer>(this, window);
    roadBuilder = std::make_unique<RoadBuilder>();
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
                // Release road point (right click)
                if (event.mouseButton.button == sf::Mouse::Button::Right
                    && state == GameState::BuildingRoad
                    && roadBuilder->popPoint()) break;

                const bool uiClick = this->gameRenderer->mouseClickEvent(event);
                if (uiClick) break;

                if (state == GameState::BuildingRoad) {
                    roadBuilder->addPoint();

                    if (roadBuilder->total() == 2 && roadBuilder->getMode() == STRAIGHT) {
                        roadBuilder->buildSegment(*road);
                    } else if (roadBuilder->total() == 3 && roadBuilder->getMode() == CURVED) {
                        roadBuilder->buildSegment(*road);
                    }
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

                roadBuilder->setMousePosition(window->mapPixelToCoords(
                    sf::Mouse::getPosition(*window), gameRenderer->getCamera()
                ), *road);

                break;
            }

            default: break;
        }
    }
}
