#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include "utilities.hpp"
#include "point.hpp"

enum RayHitAxis {
    none,
    vertical,
    horizontal
};

class Ray2D {
    public:
        Ray2D(Point2D& origin, double angleRad, int dof, double cellSize, std::vector<std::vector<char>>& grid) {
            this->origin = origin;
            rayAngleRad = wrapRadAngle(angleRad);
            maxDof = dof;
            this->cellSize = cellSize;
            this->grid = grid;
            rayCast();
        }

        const double getLength() const {
            return length;
        }

        const bool getHit() const {
            return hit;
        }

        const Point2D getOrigin() const {
            return origin;
        }

        const Point2D getHitPos() const {
            return hitPos;
        }

        // Returns the gridline axis that the ray collided with
        const RayHitAxis getHitAxis() {
            return hitAxis;
        }

        // https://www.youtube.com/watch?v=gYRrGTC7GtA
        void rayCast() {
            // DDA (Digital Differential Analysis)
            // - find offsets
            // - loop through dof using offsets to check intersections with hori or vert grid lines
            // - complete for both axis
            // - use the shorter ray of the two (collided earlier)

            int gridX, gridY;  // ray hit point coordinates normalised to grid coords
            double rayX, rayY;  // hitting point of the ray
            double xOffset, yOffset;  // offset to jump to next vert or hori grid line (same every jump)
            double horiX, horiY;  // cache hori ray hit point while computing vert

            bool horiHit, vertHit;
            double vertRayDist, horiRayDist;  // store length of both rays for comparison
            double infiniteDist = cellSize * maxDof + 1;  // largest distance possible in dof + 1

            double negTan = -tan(rayAngleRad);
            double atan = -1 / tan(rayAngleRad);

            // -- horizontal grid lines check --
            int dof = 0;
            // looking up
            if (rayAngleRad > M_PI/2 && rayAngleRad < 3*M_PI/2) {
                rayY = (((int) origin.y()) / cellSize) * cellSize - 0.0000001;  // snap y to grid (account for float math error)
                rayX = (origin.y() - rayY) * negTan + origin.x();
                yOffset = -cellSize;
                xOffset = -yOffset * negTan;
            // looking down
            } else if (rayAngleRad > 3*M_PI/2 || rayAngleRad < M_PI/2) {
                rayY = (((int) origin.y()) / cellSize) * cellSize + cellSize;  // snap y to grid
                rayX = (origin.y() - rayY) * negTan + origin.x();
                yOffset = cellSize;
                xOffset = -yOffset * negTan;
            // looking straight right or left, skip and use other ray
            } else {
                dof = maxDof;
                horiRayDist = infiniteDist;
            }
            while (dof < maxDof) {
                gridX = (int) rayX / cellSize;
                gridY = (int) rayY / cellSize;
                // has hit (is within grid and cell is filled)
                if (gridX >= 0 && gridY >= 0 && gridX < grid[0].size() && gridY < grid.size() && grid[gridY][gridX] == '#') {
                    horiHit = true;
                    break;
                // check next horizontal grid line
                } else {
                    dof++;
                    if (dof < maxDof) {
                        rayX += xOffset;
                        rayY += yOffset;
                    }
                }
            }
            if (horiRayDist != infiniteDist) {
                horiRayDist = origin.getDistance(Point2D(rayX, rayY));
            }
            horiX = rayX;
            horiY = rayY;

            // -- vertical grid lines check --
            dof = 0;
            // looking left
            if (rayAngleRad > M_PI) {
                rayX = (((int) origin.x()) / cellSize) * cellSize - 0.0000001;  // snap y to grid (account for float math error)
                rayY = (origin.x() - rayX) * atan + origin.y();
                xOffset = -cellSize;
                yOffset = -xOffset * atan;
            // looking right
            } else if (rayAngleRad < M_PI && rayAngleRad != 0) {
                rayX = (((int) origin.x()) / cellSize) * cellSize + cellSize;  // snap y to grid
                rayY = (origin.x() - rayX) * atan + origin.y();
                xOffset = cellSize;
                yOffset = -xOffset * atan;
            // looking straight up or down, skip and use other ray
            } else {
                dof = maxDof;
                vertRayDist = infiniteDist;
            }
            while (dof < maxDof) {
                gridX = (int) rayX / cellSize;
                gridY = (int) rayY / cellSize;
                // has hit (is within grid and cell is filled)
                if (gridX >= 0 && gridY >= 0 && gridX < grid[0].size() && gridY < grid.size() && grid[gridY][gridX] == '#') {
                    vertHit = true;
                    break;
                // check next horizontal grid line
                } else {
                    dof++;
                    if (dof < maxDof) {
                        rayX += xOffset;
                        rayY += yOffset;
                    }
                }
            }
            if (vertRayDist != infiniteDist) {
                vertRayDist = origin.getDistance(rayX, rayY);
            }

            // return shorter ray (collided with wall earlier)
            if (horiRayDist < vertRayDist) {
                hit = horiHit;
                hitPos.set(horiX, horiY);
                length = horiRayDist;
                hitAxis = RayHitAxis::horizontal;
            } else {
                hit = vertHit;
                hitPos.set(rayX, rayY);
                length = vertRayDist;
                hitAxis = RayHitAxis::vertical;
            }
        }



    private:
        int cellSize;
        std::vector<std::vector<char>> grid;
        int maxDof;
        double rayAngleRad;

        double length = 0;
        bool hit = false;
        RayHitAxis hitAxis = RayHitAxis::none;
        Point2D origin = Point2D(0, 0);
        Point2D hitPos = Point2D(0, 0);

};
