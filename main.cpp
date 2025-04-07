#include <iostream>
#include <SFML/Graphics.hpp>

#include "Game.h"


int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Traffix");

    Game game = Game(&window);
    while (window.isOpen()) {
        game.update();
        game.render();
    }

    return 0;
}