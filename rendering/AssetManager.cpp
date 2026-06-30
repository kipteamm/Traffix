#include <stdexcept>

#include "AssetManager.h"


AssetManager::AssetManager() {
    if (!font.loadFromFile("../assets/fonts/Montserrat.ttf"))
        throw std::runtime_error("Could not load 'assets/fonts/Montserrat.ttf' asset");

    if (!roadTexture.loadFromFile("../assets/textures/road.jpg"))
        throw std::runtime_error("Could not load 'assets/textures/road.jpg' asset");

    roadTexture.setRepeated(true);
}

AssetManager& AssetManager::getInstance() {
    // The instance is created the first time this function is called.
    // It is destroyed automatically when the program exits.
    static AssetManager instance;
    return instance;
}


sf::Font &AssetManager::getFont() {
    return font;
}


sf::Texture &AssetManager::getRoadTexture() {
    return roadTexture;
}
