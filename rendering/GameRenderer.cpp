#include "GameRenderer.h"


GameRenderer::GameRenderer(Game *game, const std::shared_ptr<sf::RenderWindow>& window) : game(game), window(window) {
    camera = window->getDefaultView();
    uiManager = new UIManager(game, *window);

    const sf::Vector2u size = window->getSize();
    const float width = static_cast<float>(size.x);
    const float height = static_cast<float>(size.y);
    ui.setSize(width, height);
    ui.setCenter(width / 2, height / 2);

    window->setFramerateLimit(60);
}


void GameRenderer::render() {
    uiManager->update(game->getState());

    handleMouseDrag();

    window->clear(sf::Color(64, 156, 12));
    window->setView(camera);
    renderWorld();

    window->setView(ui);
    uiManager->render();

    window->display();
}

void GameRenderer::renderWorld() {
    if (game->getState() == GameState::BuildingRoad) {
        const sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(100, 100), sf::Color::White),
                sf::Vertex(sf::Vector2f(400, 450), sf::Color::White)
        };
        window->draw(line, 2, sf::Lines);
    }
}


void GameRenderer::resizeEvent(const sf::Event& event) {
    const float width = static_cast<float>(event.size.width);
    const float height = static_cast<float>(event.size.height);

    camera.setSize(width * zoom, height * zoom);
    ui.setSize(width, height);
    ui.setCenter(width / 2, height / 2);

    uiManager->handleResize();
}

void GameRenderer::mouseWheelEvent(const sf::Event &event) {
    if (event.mouseWheelScroll.delta > 0) {
        zoom *= 0.9f;
    } else {
        zoom *= 1.1f;
    }

    const sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
    const sf::Vector2f beforeZoom = window->mapPixelToCoords(pixelPos, camera);

    camera.setSize(static_cast<float>(window->getSize().x) * zoom, static_cast<float>(window->getSize().y) * zoom);

    const sf::Vector2f afterZoom = window->mapPixelToCoords(pixelPos, camera);
    const sf::Vector2f offset = beforeZoom - afterZoom;

    camera.move(offset);
}

void GameRenderer::mouseClickEvent(const sf::Event &event) {
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
        dragging = true;
        dragStart = sf::Mouse::getPosition(*window);
        return;
    }

    if (event.mouseButton.button == sf::Mouse::Button::Left) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
        sf::Vector2f uiPos = window->mapPixelToCoords(pixelPos, ui);

        uiManager->handleClick(uiPos);

        // You might also want to handle clicks on the world (camera view)
        // sf::Vector2f worldPos = window->mapPixelToCoords(pixelPos, camera);
        // e.g., if (game->getState() == GameState::BuildingRoad) { ... }
    }
}

void GameRenderer::mouseReleaseEvent(const sf::Event &event) {
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
        dragging = false;
    }
}

void GameRenderer::handleMouseDrag() {
    if (!dragging) return;

    const sf::Vector2i currentPos = sf::Mouse::getPosition(*window);
    const sf::Vector2f worldDelta = window->mapPixelToCoords(dragStart, camera) - window->mapPixelToCoords(currentPos, camera);
    camera.move(worldDelta);

    dragStart = currentPos;
}
