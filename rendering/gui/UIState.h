#ifndef TRAFFIX_UISTATE_H
#define TRAFFIX_UISTATE_H

#include "../../Game.h"
#include "UIButton.h"


class Game;
class UIButton;


enum UIAnchor {
    TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT
};


class UIState {
public:
    explicit UIState(UIAnchor anchor, float paddingX, float paddingY);

    void render(sf::RenderWindow& window);
    bool handleClick(sf::Vector2f mousePos, Game* game);
    void handleResize(const sf::Vector2u& size);

    virtual ~UIState() = default;

protected:
    std::vector<UIButton> buttons;

private:
    UIAnchor anchor;
    float paddingX;
    float paddingY;
};


class DefaultUI final : public UIState {
public:
    DefaultUI(const sf::Window &window);
};


class RoadBuildUI final : public UIState {
public:
    RoadBuildUI(const sf::Window &window);
};


class BuildingRoadUI final : public UIState {
public:
    BuildingRoadUI(const sf::Window &window);
};


#endif //TRAFFIX_UISTATE_H