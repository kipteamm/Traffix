#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <unordered_map>
#include <memory>

#include "../../Game.h"
#include "UIState.h"


class Game;
class UIState;
enum class GameState;


class UIManager {
public:
    explicit UIManager(Game* game, sf::RenderWindow &window);

    void update(GameState state);
    void render() const;

    void handleResize() const;
    bool handleClick(sf::Vector2f mousePos) const;

private:
    Game* game;
    sf::RenderWindow &window;

    std::unordered_map<GameState, std::unique_ptr<UIState>> modes;
    UIState* active = nullptr;
};



#endif //UIMANAGER_H
