// only include in compilation once
#pragma once

#include <vector>
#include <functional>

class Point2D {
    public:
        double x;
        double y;

        Point2D() {
            x = 0;
            y = 0;
        }

        Point2D(int pX, int pY) {
            x = (double) pX;
            y = (double) pY;
        }

        Point2D(double pX, double pY) {
            x = pX;
            y = pY;
        }

        Point2D(const Point2D& p) {
            x = p.x;
            y = p.y;
        }

        bool operator==(const Point2D& p) {
            return x == p.x && y == p.y;
        }

        bool operator!=(const Point2D& p) {
            return !(*this == p);
        }

        Point2D operator+(const Point2D& p) {
            return Point2D(x + p.x, y + p.y);
        }

        Point2D operator-(const Point2D& p) {
            return Point2D(x - p.x, y - p.y);
        }

        void set(int pX, int pY) {
            x = (double) pX;
            y = (double) pY;
        }

        void set(double pX, double pY) {
            x = pX;
            y = pY;
        }

        void set(const Point2D& p) {
            x = p.x;
            y = p.y;
        }

        std::vector<Point2D> getCardinalNeighbours() {
            std::vector<Point2D> neighbourOffsets = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
            std::vector<Point2D> cardinalNeighbours;
            for (auto offset : neighbourOffsets) {
                cardinalNeighbours.push_back(*this + offset);
            }
            return cardinalNeighbours;
        }

        std::vector<Point2D> getDiagonalNeighbours() {
            std::vector<Point2D> neighbourOffsets = {{1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
            std::vector<Point2D> diagonalNeighbours;
            for (auto offset : neighbourOffsets) {
                diagonalNeighbours.push_back(*this + offset);
            }
            return diagonalNeighbours;
        }

        std::vector<Point2D> getAllNeighbours() {
            std::vector<Point2D> cardinalNeighbours = getCardinalNeighbours();
            std::vector<Point2D> diagonalNeighbours = getDiagonalNeighbours();
            // splice vectors and return result
            cardinalNeighbours.insert(cardinalNeighbours.end(), diagonalNeighbours.begin(), diagonalNeighbours.end());
            return cardinalNeighbours;
        }

    private:
};


// need binary operator overloads outside of class as well??
// https://stackoverflow.com/questions/2828280/friend-functions-and-operator-overloading-what-is-the-proper-way-to-overlo
bool operator==(const Point2D& p1, const Point2D& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(const Point2D& p1, const Point2D& p2) {
    return p1.x != p2.x || p1.y != p2.y;
}


// allows points to be hashed for unordered_map
class PointHasher {
    public:
        size_t operator()(const Point2D& p) const {
            return std::hash<double>()(p.x) ^ (std::hash<double>()(p.y) << 1);
        }
};