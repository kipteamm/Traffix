#ifndef TRAFFIX_INTERSECTION_H
#define TRAFFIX_INTERSECTION_H

#include <SFML/Graphics.hpp>


struct Node;

class Intersection {
public:
    explicit Intersection(Node* centerNode);

    void buildMesh();

    [[nodiscard]] const sf::VertexArray& getAsphaltMesh() const { return asphaltMesh; }
    [[nodiscard]] const sf::VertexArray& getMarkingsMesh() const { return markingsMesh; }
    [[nodiscard]] sf::FloatRect getBounds() const { return asphaltMesh.getBounds(); }

private:
    Node* centerNode;
    sf::VertexArray asphaltMesh;
    sf::VertexArray markingsMesh;

    void generateAsphaltMesh();
    void generateMarkingsMesh();
};


#endif //TRAFFIX_INTERSECTION_H