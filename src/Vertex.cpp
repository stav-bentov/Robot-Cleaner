#include "../simulator/include/vertex.h"

void Vertex::setDirt(int _dirt) {
    dirt = _dirt;
    isDirtKnown = true;
}

void Vertex::setVisited() {
    visited = true;
}

bool Vertex::getVisited() const{
    return visited;
}

void Vertex::reduceDirt() {
    dirt--;
}

int Vertex::getDirt() const {
    return dirt;
}

bool Vertex::isWall() const {
    return type == Type::Wall;
}

void Vertex::updateType(Type t) {
    type = t;
}

std::pair<int, int> Vertex::getLocation() const{
    return std::make_pair(row, col);
}

bool Vertex::getIsDirtKnown() const {
    return isDirtKnown;
}