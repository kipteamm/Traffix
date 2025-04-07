//
// Created by PPetre on 7/04/2025.
//

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <unordered_map>
#include <functional>
#include <memory>

#include "Game.h"


enum class GameState;
class Game;


class UIButton {
public:
    UIButton(const sf::Vector2f& size, const sf::Font& font, const std::string& text);

    void setPosition(const sf::Vector2f& pos);
    void setColor(const sf::Color& color);
    void setOnClick(std::function<void(Game*)> action);
    bool contains(const sf::Vector2f& pos) const;
    void draw(sf::RenderWindow& window) const;

    void onClick(Game* game) const;

private:
    sf::RectangleShape shape;
    sf::Text label;

    std::function<void(Game*)> callback;
};


class UIState {
public:
    void render(sf::RenderWindow& window);
    void handleClick(sf::Vector2f mousePos, Game* game);

    virtual ~UIState() = default;

protected:
    std::vector<UIButton> buttons;
};

class DefaultUI : public UIState {
    DefaultUI(const sf::Window &window, const sf::Font &font);
};

class RoadBuildUI : public UIState {
    RoadBuildUI(const sf::Window &window, const sf::Font &font);
};


class UIManager {
public:
    explicit UIManager(Game* game, sf::RenderWindow &window);

    void update(GameState state);
    void render() const;
    void handleClick(sf::Vector2f mousePos) const;

private:
    Game* game;
    sf::RenderWindow &window;
    sf::Font font;

    std::unordered_map<GameState, std::unique_ptr<UIState>> modes;
    UIState* active = nullptr;
};



#endif //UIMANAGER_H
