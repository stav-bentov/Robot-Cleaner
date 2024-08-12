#ifndef VERTEX_H_
#define VERTEX_H_

#include <unordered_set>
#include <memory>
#include <vector>
#include "common_enums.h"

class Vertex {
    private:
        int row;
        int col;
        Type type;
        int distanceFromDockingStation;
        int dirt;
        bool isDirtKnown;
        bool visited;
    public:
        Vertex(std::pair<int, int> location, Type t);
        
        bool operator==(const Vertex &other) const {
            return row == other.row && col == other.col;
        }

        void setDirt(int _dirt);
        int getDirt() const;
        void reduceDirt();
        void updateType(Type t);
        bool isWall() const;
        std::pair<int, int> getLocation() const;
        bool getIsDirtKnown() const;
        void setVisited();
        bool getVisited() const;
};
#endif  // VERTEX_H_