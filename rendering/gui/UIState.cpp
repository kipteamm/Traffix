#include "UIState.h"

#include <iostream>

#include "../AssetManager.h"


UIState::UIState(const UIAnchor anchor, const float paddingX, const float paddingY)
    : anchor(anchor), paddingX(paddingX), paddingY(paddingY) {}



bool UIState::handleClick(const sf::Vector2f mousePos, Game* game) {
    for (UIButton &button: buttons) {
        if (!button.contains(mousePos)) continue;
        button.onClick(game);
        return true;
    }

    return false;
}


void UIState::render(sf::RenderWindow &window) {
    for (UIButton &button: buttons) {
        button.draw(window);
    }
}


void UIState::handleResize(const sf::Vector2u& size) {
    float currentX = 10.f;
    float currentY = 10.f;

    switch (anchor) {
        case TOP_LEFT:
            currentX = 10.f; break;

        case TOP_RIGHT:
            currentX = size.x - 10.f; break;

        case BOTTOM_LEFT:
            currentX = 10.f;
            currentY = size.y - 10.f; break;

        case BOTTOM_RIGHT:
            currentX = size.x - 10.f;
            currentY = size.y - 10.f; break;
    }

    for (UIButton &button: buttons) {
        button.setPosition(sf::Vector2f(currentX, currentY - button.getSize().y));

        currentX += paddingX;
        currentY += paddingY;
    }
}



DefaultUI::DefaultUI(const sf::Window &window)
    : UIState(BOTTOM_LEFT, 50.f, 0)
{
    const sf::Vector2f buttonSize(40.f, 40.f);

    UIButton roadButton(buttonSize, "R", sf::Color(192, 192, 192));

    const sf::Vector2u windowSize = window.getSize();
    const sf::Vector2f position(10.f, windowSize.y - buttonSize.y - 10.f);
    roadButton.setPosition(position);

    roadButton.setOnClick([](Game* game) {
        std::cout << "Road button clicked!" << std::endl;
        game->setState(GameState::RoadBuildMenu);
    });

    buttons.push_back(roadButton);

    this->handleResize(window.getSize());
}


RoadBuildUI::RoadBuildUI(const sf::Window &window)
    : UIState(BOTTOM_LEFT, 50.f, 0)
{
    const sf::Vector2f buttonSize(40.f, 40.f);

    UIButton oneWayRoadButton(buttonSize, "1", sf::Color(192, 192, 192));
    UIButton twoWayRoadButton(buttonSize, "2", sf::Color(192, 192, 192));

    oneWayRoadButton.setOnClick([](Game* game){
        std::cout << "One-way road selected!" << std::endl;
        game->setState(GameState::BuildingRoad);
    });

    twoWayRoadButton.setOnClick([](Game* game){
        std::cout << "Two-way road selected!" << std::endl;
        game->setState(GameState::BuildingRoad);
    });

    buttons.push_back(oneWayRoadButton);
    buttons.push_back(twoWayRoadButton);

    this->handleResize(window.getSize());
}


BuildingRoadUI::BuildingRoadUI(const sf::Window& window)
    : UIState(BOTTOM_LEFT, 50.f, 0)
{
    const sf::Vector2f buttonSize(40.f, 40.f);

    UIButton straightRoadButton(buttonSize, "S", sf::Color(192, 192, 192));
    UIButton curvedRoadButton(buttonSize, "C", sf::Color(192, 192, 192));

    straightRoadButton.setOnClick([](Game* game){
        std::cout << "Straight road selected!" << std::endl;
        game->getRoadBuilder()->setMode(STRAIGHT);
    });

    curvedRoadButton.setOnClick([](Game* game){
        std::cout << "Curved road selected!" << std::endl;
        game->getRoadBuilder()->setMode(CURVED);
    });

    buttons.push_back(straightRoadButton);
    buttons.push_back(curvedRoadButton);

    this->handleResize(window.getSize());
}
