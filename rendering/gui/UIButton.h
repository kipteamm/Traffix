#ifndef TRAFFIX_UIBUTTON_H
#define TRAFFIX_UIBUTTON_H


#include <functional>

#include "../../Game.h"


class Game;
enum class GameState;


class UIButton {
public:
    explicit UIButton(const sf::Vector2f& size, const std::string& text, const sf::Color &color);

    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getSize() const;

    void setOnClick(std::function<void(Game*)> action);
    bool contains(const sf::Vector2f& pos) const;
    void draw(sf::RenderWindow& window) const;

    void onClick(Game* game) const;

private:
    sf::RectangleShape shape;
    sf::Text label;

    std::function<void(Game*)> callback;
};


#endif //TRAFFIX_UIBUTTON_H