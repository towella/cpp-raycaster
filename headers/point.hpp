// only include in compilation once
#pragma once

#include <vector>
#include <functional>

class Point {
    public:
        int x;
        int y;

        Point() {
            x = 0;
            y = 0;
        }
        Point(int pX, int pY) {
            x = pX;
            y = pY;
        }

        Point(const Point& p) {
            x = p.x;
            y = p.y;
        }

        bool operator==(const Point& p) {
            return x == p.x && y == p.y;
        }

        bool operator!=(const Point& p) {
            return !(*this == p);
        }

        Point operator+(const Point& p) {
            return Point(x + p.x, y + p.y);
        }

        Point operator-(const Point& p) {
            return Point(x - p.x, y - p.y);
        }

        void set(int pX, int pY) {
            x = pX;
            y = pY;
        }

        void set(const Point& p) {
            x = p.x;
            y = p.y;
        }

        std::vector<Point> getCardinalNeighbours() {
            std::vector<Point> neighbourOffsets = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
            std::vector<Point> cardinalNeighbours;
            for (auto offset : neighbourOffsets) {
                cardinalNeighbours.push_back(*this + offset);
            }
            return cardinalNeighbours;
        }

        std::vector<Point> getDiagonalNeighbours() {
            std::vector<Point> neighbourOffsets = {{1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
            std::vector<Point> diagonalNeighbours;
            for (auto offset : neighbourOffsets) {
                diagonalNeighbours.push_back(*this + offset);
            }
            return diagonalNeighbours;
        }

        std::vector<Point> getAllNeighbours() {
            std::vector<Point> cardinalNeighbours = getCardinalNeighbours();
            std::vector<Point> diagonalNeighbours = getDiagonalNeighbours();
            // splice vectors and return result
            cardinalNeighbours.insert(cardinalNeighbours.end(), diagonalNeighbours.begin(), diagonalNeighbours.end());
            return cardinalNeighbours;
        }

    private:
};


// need binary operator overloads outside of class as well??
// https://stackoverflow.com/questions/2828280/friend-functions-and-operator-overloading-what-is-the-proper-way-to-overlo
bool operator==(const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(const Point& p1, const Point& p2) {
    return p1.x != p2.x || p1.y != p2.y;
}


// allows points to be hashed for unordered_map
class PointHasher {
    public:
        size_t operator()(const Point& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
};