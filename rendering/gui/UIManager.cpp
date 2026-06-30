#include "UIManager.h"

#include <cassert>
#include <iostream>


UIManager::UIManager(Game* game, sf::RenderWindow &window): window(window) {
    if (!font.loadFromFile("../assets/fonts/Montserrat.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return;
    }
    
    this->game = game;
    modes[GameState::Default] = std::make_unique<DefaultUI>(window, font);
    modes[GameState::RoadBuildMenu] = std::make_unique<RoadBuildUI>(window, font);
    modes[GameState::BuildingRoad] = std::make_unique<UIState>(BOTTOM_LEFT, 0, 0);
    
    active = modes[GameState::Default].get();
}


void UIManager::update(const GameState state) {
    if (modes.count(state)) {
        active = modes[state].get();
    } else {
        active = nullptr;
    }
}


void UIManager::render() const {
    assert(active != nullptr && "UIManager does not have active UIState");
    active->render(window);
}


void UIManager::handleResize() const {
    assert(active != nullptr && "UIManager does not have active UIState");
    active->handleResize(window.getSize());
}



bool UIManager::handleClick(const sf::Vector2f mousePos) const {
    assert(active != nullptr && "UIManager does not have active UIState");
    return active->handleClick(mousePos, game);
}