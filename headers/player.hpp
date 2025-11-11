#pragma once

#include <iostream>
#include <vector>

#include "window.hpp"
#include "point.hpp"

#define EMPTY '.'


class Player : public Point2D {
    public:
        Player() {
            _x = 0;
            _y = 0;
            setupCamera();
        }

        Player(int pX, int pY) {
            _x = (double) pX;
            _y = (double) pY;
            setupCamera();
        }

        Player(double pX, double pY) {
            _x = pX;
            _y = pY;
            setupCamera();
        }

        Player(const Point2D& p) {
            _x = p.x();
            _y = p.y();
            setupCamera();
        }

        const double x() const {
            return _x;
        }

        const double y() const {
            return _y;
        }

        const double getRotRad() const {
            return rotRad;
        }

        const double getFovRad() const {
            return (double) cameraPlaneDist / cameraPlaneWidth * 100 * M_PI/180;
        }

        const std::pair<Point2D, Point2D> getCameraPlane() const {
            return cameraPlane;
        }

        void set(double pX, double pY) {
            Point2D offset(pX - _x, pY - _y);
            _x = pX;
            _y = pY;
            cameraPlane.first = cameraPlane.first + offset;
            cameraPlane.second = cameraPlane.second + offset;
        }

        void set(int pX, int pY) {
            set((double) pX, (double) pY);
        }

        void set(const Point2D& p) {
            set(p.x(), p.y());
        }

        void update(double dt, std::vector<std::vector<char>> map, int wallSize) {
            double prevRotRad = rotRad; 
            Point2D movedPlayer = getInput(dt);

            // check player collisions
            Point2D mapNormalisedPlayer((int) movedPlayer.x() / wallSize, (int) movedPlayer.y() / wallSize);
            bool inMap = mapNormalisedPlayer.x() >= 0 && mapNormalisedPlayer.y() >= 0 && 
                         mapNormalisedPlayer.x() < map[0].size() && mapNormalisedPlayer.y() < map.size();
            if (inMap && map[mapNormalisedPlayer.y()][mapNormalisedPlayer.x()] == EMPTY) {
                set(movedPlayer);
            }

            // rotate camera with player
            cameraPlane.first.rotateRad(*this, rotRad - prevRotRad);
            cameraPlane.second.rotateRad(*this, rotRad - prevRotRad);
        }

        void draw2D(Window& window) const {
            SDL_Rect playerPoint = {(int) _x, (int) _y, 5, 5};
            window.renderRect(playerPoint, Colours::red);

            Point2D debugp((Point2D) *this + Point2D(0, cameraPlaneDist));  // directly below at 0 rad
            window.renderLine(*this, debugp.rotateRad(*this, rotRad), Colours::yellow);

            window.renderLine(cameraPlane.first, cameraPlane.second, Colours::green);
        }

    private:
        // x and y unlike point should be private so can't be modified without moving camera too
        double rotRad = M_PI;  // 0 is down, inc anti-clockwise
        const double rotSpeed = 0.1;
        const double playerSpeed = 2;

        int cameraPlaneDist = 20;
        int cameraPlaneWidth = 30;
        std::pair<Point2D, Point2D> cameraPlane;

        void setupCamera() {
            cameraPlane.first = Point2D(_x + cameraPlaneWidth / 2, _y + cameraPlaneDist);  // left
            cameraPlane.second = Point2D(_x - cameraPlaneWidth / 2, _y + cameraPlaneDist);  // right
            // orient to player init rotation
            cameraPlane.first.rotateRad(*this, rotRad);
            cameraPlane.second.rotateRad(*this, rotRad);
        }

        Point2D getInput(double dt) {
            std::unordered_map<SDL_Keycode, bool> keysPressed = Input::getPressed();

            // account for diagonal movement
            double framePlayerSpeed;
            if ((keysPressed[SDLK_a] || keysPressed[SDLK_d]) && (keysPressed[SDLK_w] || keysPressed[SDLK_s])) {
                framePlayerSpeed = playerSpeed / 1.414 * dt;
            } else {
                framePlayerSpeed = playerSpeed * dt;
            }

            // movement input (relative to rotation)
            Point2D moveOffset;
            if (keysPressed[SDLK_w]) {
                moveOffset = Point2D(sin(rotRad) * framePlayerSpeed, cos(rotRad) * framePlayerSpeed);
            }
            if (keysPressed[SDLK_a]) {
                moveOffset = moveOffset + Point2D(sin(rotRad + M_PI/2) * framePlayerSpeed, cos(rotRad + M_PI/2) * framePlayerSpeed);
            }
            if (keysPressed[SDLK_s]) {
                moveOffset = moveOffset + Point2D(sin(rotRad + M_PI) * framePlayerSpeed, cos(rotRad + M_PI) * framePlayerSpeed);
            }
            if (keysPressed[SDLK_d]) {
                moveOffset = moveOffset + Point2D(sin(rotRad - M_PI/2) * framePlayerSpeed, cos(rotRad - M_PI/2) * framePlayerSpeed);
            }
            Point2D movedPlayer = (*this) + moveOffset;

            // rotation input
            double frameRotSpeed = rotSpeed * dt;
            if (keysPressed[SDLK_RIGHT]) {
                rotRad -= frameRotSpeed;
            }
            if (keysPressed[SDLK_LEFT]) {
                rotRad += frameRotSpeed;
            }
            rotRad = wrapRadAngle(rotRad);

            return movedPlayer;
        }
};