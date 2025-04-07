//
// Created by kipteam on 4/6/25.
//

#include "GameRenderer.h"


GameRenderer::GameRenderer(Game *game, sf::RenderWindow *window) : game(game), window(window) {
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
    handleEvents();
    handleMouseDrag();

    window->clear(sf::Color(64, 156, 12));
    window->setView(camera);
    renderWorld();

    window->setView(ui);
    uiManager->render();

    window->display();
}


void GameRenderer::handleEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::Resized:
                windowViewEvent(event);
                break;
            case sf::Event::MouseButtonPressed:
                mouseClickEvent(event);
                break;
            case sf::Event::MouseButtonReleased:
                mouseReleaseEvent(event);
                break;
            case sf::Event::MouseWheelMoved:
                mouseWheelEvent(event);
                break;
            default:
                break;
        }
    }
}

void GameRenderer::windowViewEvent(const sf::Event& event) {
    const float width = event.size.width;
    const float height = event.size.height;

    camera.setSize(width * zoom, height * zoom);
    ui.setSize(width, height);
    ui.setCenter(width / 2, height / 2);
}

void GameRenderer::mouseWheelEvent(const sf::Event &event) {
    if (event.mouseWheelScroll.delta > 0) {
        zoom *= 0.9f;
    } else {
        zoom *= 1.1f;
    }

    const sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
    const sf::Vector2f beforeZoom = window->mapPixelToCoords(pixelPos, camera);

    camera.setSize(window->getSize().x * zoom, window->getSize().y * zoom);

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

    // left
}

void GameRenderer::mouseReleaseEvent(const sf::Event &event) {
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
        dragging = false;
    }
}

void GameRenderer::handleMouseDrag() {
    if (!dragging) return;

    const sf::Vector2i currentPos = sf::Mouse::getPosition();
    const sf::Vector2f worldDelta = window->mapPixelToCoords(dragStart) - window->mapPixelToCoords(currentPos);
    camera.move(worldDelta);

    dragStart = currentPos;
}

