//
// Created by PPetre on 7/04/2025.
//

#include "UIManager.h"

#include <iostream>


UIButton::UIButton(const sf::Vector2f &size, const sf::Font &font, const std::string &text) {
    shape.setSize(size);
    label.setFont(font);
    label.setString(text);
}

void UIButton::setPosition(const sf::Vector2f &pos) {
    shape.setPosition(pos);
    label.setPosition(pos);
}

void UIButton::setOnClick(std::function<void(Game*)> action) {
    callback = std::move(action);
}

void UIButton::onClick(Game* game) const {
    if (callback) callback(game);
}


void UIState::handleClick(const sf::Vector2f mousePos, Game* game) {
    for (UIButton &button: buttons) {
        if (!button.contains(mousePos)) continue;
        button.onClick(game);
    }
}

void UIState::render(sf::RenderWindow &window) {
    for (UIButton &button: buttons) {
        button.draw(window);
    }
}


DefaultUI::DefaultUI(const sf::Window &window, const sf::Font &font) {
    const sf::Vector2f buttonSize(40.f, 40.f);

    UIButton roadButton(buttonSize, font, "R");
    roadButton.setColor(sf::Color(192, 192, 192));

    const sf::Vector2u windowSize = window.getSize();
    const sf::Vector2f position(10.f, windowSize.y - buttonSize.y - 10.f);
    roadButton.setPosition(position);

    roadButton.setOnClick([](Game* game) {
        game->setState(GameState::BuildingRoad);
    });

    buttons.push_back(roadButton);
}


RoadBuildUI::RoadBuildUI(const sf::Window &window, const sf::Font &font) {
    const sf::Vector2f buttonSize(40.f, 40.f);

    UIButton oneWayRoadButton(buttonSize, font, "1");
    UIButton twoWayRoadButton(buttonSize, font, "2");
    oneWayRoadButton.setColor(sf::Color(192, 192, 192));
    twoWayRoadButton.setColor(sf::Color(192, 192, 192));

    const sf::Vector2u windowSize = window.getSize();
    sf::Vector2f position(10.f, windowSize.y - buttonSize.y - 10.f);
    oneWayRoadButton.setPosition(position);

    position.x = 60.f;
    position.y = windowSize.y - buttonSize.y - 10.f;
    oneWayRoadButton.setPosition(position);

    buttons.push_back(oneWayRoadButton);
    buttons.push_back(twoWayRoadButton);
}


UIManager::UIManager(Game* game, sf::RenderWindow &window): window(window) {
    modes[GameState::Default] = std::make_unique<DefaultUI>(window, font);
    modes[GameState::RoadBuildMenu] = std::make_unique<RoadBuildUI>(window, font);
    active = modes[GameState::Default].get();
    this->game = game;

    if (!font.loadFromFile("../assets/fonts/Montserrat.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return;
    }
}


void UIManager::update(const GameState state) {
    active = modes[state].get();
}

void UIManager::render() const {
    if (active) active->render(window);
}

void UIManager::handleClick(const sf::Vector2f mousePos) const {
    if (active) active->handleClick(mousePos, game);
}
