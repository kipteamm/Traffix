#include <SFML/Graphics.hpp>

#include "Game.h"


int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "TrafficSimulator");
    sf::View view = window.getDefaultView();

    Game game = Game(&window, &view);

    while (window.isOpen()) {
        game.update();
        game.render();
    }

    return 0;
}