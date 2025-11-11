// only include in compilation once
#pragma once

#include <vector>
#include <functional>
#include "utilities.hpp"

class Point2D {
    public:
        Point2D() {
            _x = 0;
            _y = 0;
        }

        Point2D(int pX, int pY) {
            _x = (double) pX;
            _y = (double) pY;
        }

        Point2D(double pX, double pY) {
            _x = pX;
            _y = pY;
        }

        Point2D(const Point2D& p) {
            _x = p._x;
            _y = p._y;
        }

        const double x() const {
            return _x;
        }

        const double y() const {
            return _y;
        }

        bool operator==(const Point2D& p) {
            return _x == p._x && _y == p._y;
        }

        bool operator!=(const Point2D& p) {
            return !(*this == p);
        }

        Point2D operator+(const Point2D& p) {
            return Point2D(_x + p._x, _y + p._y);
        }

        Point2D operator-(const Point2D& p) {
            return Point2D(_x - p._x, _y - p._y);
        }

        void set(double pX, double pY) {
            _x = pX;
            _y = pY;
        }

        void set(int pX, int pY) {
            set((double) pX, (double) pY);
        }

        void set(const Point2D& p) {
            set(p._x, p._y);
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

        double getDistance(const double x, const double y) {
            return abs(sqrt((x - _x)*(x - _x) + (y - _y)*(y - _y)));
        }

        double getDistance(const Point2D& p) {
            return getDistance(p._x, p._y);
        }

        double getAngleTo(const Point2D& p) {
            // -atan2 to fix flipped y I think?
            // M_PI/2 to bring angle so 0 rad is down rather than right
            return wrapRadAngle(-atan2(p._y - _y, p._x - _x) + M_PI/2);
        }

        // 0 is down, inc anti-clockwise
        Point2D rotateRad(const Point2D& origin, double rad) {
            set(*this - origin);  // relative coords
            // rotate
            double cosRot = cos(rad);
            double sinRot = sin(rad);
            set(_x * cosRot + _y * sinRot,
                _y * cosRot - _x * sinRot);
            set(*this + origin);  // back to normal coords
            return *this;
        }

    protected:
        double _x;
        double _y;

    private:
};


// need binary operator overloads outside of class as well??
// https://stackoverflow.com/questions/2828280/friend-functions-and-operator-overloading-what-is-the-proper-way-to-overlo
bool operator==(const Point2D& p1, const Point2D& p2) {
    return p1.x() == p2.x() && p1.y() == p2.y();
}

bool operator!=(const Point2D& p1, const Point2D& p2) {
    return p1.x() != p2.x() || p1.y() != p2.y();
}


// allows points to be hashed for unordered_map
class PointHasher {
    public:
        size_t operator()(const Point2D& p) const {
            return std::hash<double>()(p.x()) ^ (std::hash<double>()(p.y()) << 1);
        }
};