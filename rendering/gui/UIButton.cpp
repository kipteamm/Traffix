#include "UIButton.h"

#include "../AssetManager.h"


UIButton::UIButton(const sf::Vector2f &size, const std::string &text, const sf::Color &color) {
    shape.setSize(size);
    shape.setFillColor(color);

    label.setFont(AssetManager::getInstance().getFont());
    label.setString(text);
    label.setCharacterSize(16);

    const sf::FloatRect textBounds = label.getLocalBounds();
    label.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
}

void UIButton::setPosition(const sf::Vector2f &pos) {
    shape.setPosition(pos);
    label.setPosition(pos.x + shape.getSize().x / 2.0f, pos.y + shape.getSize().y / 2.0f);
}

sf::Vector2f UIButton::getSize() const {
    return shape.getSize();
}


bool UIButton::contains(const sf::Vector2f &pos) const {
    return shape.getGlobalBounds().contains(pos);
}

void UIButton::draw(sf::RenderWindow &window) const {
    window.draw(shape);
    window.draw(label);
}

void UIButton::setOnClick(std::function<void(Game*)> action) {
    callback = std::move(action);
}

void UIButton::onClick(Game* game) const {
    if (callback) callback(game);
}