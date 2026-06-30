#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>


class AssetManager {
public:
    /**
     * Singletons should not be cloneable or assignable.
     */
    AssetManager(AssetManager &other) = delete;
    void operator=(const AssetManager &) = delete;

    /**
     * @brief Returns an instance of the AssetManager class
     * @return AssetManager instance
     */
    [[nodiscard]] static AssetManager& getInstance();


    /**
     * @brief Returns the font used throughout the project.
     * @return sf::Font
     */
    [[nodiscard]] sf::Font& getFont();


    /**
     * @brief Returns the Road Texture.
     * @return Roat texture
     */
    [[nodiscard]] sf::Texture& getRoadTexture();

private:
    AssetManager();

    sf::Font font;

    sf::Texture roadTexture;
};



#endif //ASSETMANAGER_H