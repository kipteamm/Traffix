//
// Created by kipteam on 4/6/25.
//

#include "GameRenderer.h"


GameRenderer::GameRenderer(Game *game, sf::RenderWindow *window, sf::View *view) : game(game), window(window), camera(view) {
    window->setFramerateLimit(60);
}


void GameRenderer::render() {
    handleEvents();
    handleMouseDrag();

    window->setView(*camera);
    window->clear();

    sf::RectangleShape box(sf::Vector2f(200.f, 200.f));
    box.setFillColor(sf::Color::Green);
    box.setOrigin(50.f, 50.f);
    box.setPosition(0.f, 0.f);
    window->draw(box);

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

void GameRenderer::windowViewEvent(const sf::Event& event) const {
    camera->setSize(event.size.width * zoom, event.size.height * zoom);
    // view.setCenter(event.size.width / 2.0f, event.size.height / 2.0f);
}

void GameRenderer::mouseWheelEvent(const sf::Event &event) {
    if (event.mouseWheelScroll.delta > 0) {
        zoom *= 0.9f;
    } else {
        zoom *= 1.1f;
    }

    const sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);
    const sf::Vector2f beforeZoom = window->mapPixelToCoords(pixelPos, *camera);

    camera->setSize(window->getSize().x * zoom, window->getSize().y * zoom);

    const sf::Vector2f afterZoom = window->mapPixelToCoords(pixelPos, *camera);
    const sf::Vector2f offset = beforeZoom - afterZoom;

    camera->move(offset);
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
    camera->move(worldDelta);

    dragStart = currentPos;
}

