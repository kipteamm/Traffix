#include <SFML/Graphics.hpp>

#include "Game.h"


int main() {
    auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "Traffix");
    auto game = std::make_unique<Game>(window);

    while (window->isOpen()) {
        game->update();
        game->render();
    }

    return 0;
}