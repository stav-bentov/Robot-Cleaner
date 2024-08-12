#include "../include/vertex.h"

Vertex::Vertex(std::pair<int, int> location, Type t) : row(location.first), col(location.second), type(t), distanceFromDockingStation(-1), dirt(0), isDirtKnown(false), visited(false){
    
    Logger::getInstance().log("Vertex::Vertex: " + std::to_string(dirt) +"in location:" + std::to_string(row) + ", " + std::to_string(col) + "\n", 3);
}


void Vertex::setDirt(int _dirt) {
    Logger::getInstance().log("Vertex::setDirt: " + std::to_string(_dirt) +"in location:" + std::to_string(row) + ", " + std::to_string(col) + "\n", 3);
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
    Logger::getInstance().log("Vertex::getDirt! " + std::to_string(dirt) + "\n", 3);
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
