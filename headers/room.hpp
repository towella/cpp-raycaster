#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

#include "random.hpp"
#include "utilities.hpp"
#include "window.hpp"
#include "ray.hpp"
#include "point.hpp"
#include "input.hpp"
#include "player.hpp"

#define EMPTY '.'
#define WALL '#'
#define PLAYER 'P'

class Room {
    public:
        const int wallSize = 50;  // size of wall in pixels

        Room() {
            maxWidth = 20;
            maxHeight = 20;
            setupRoom();
        }

        Room(const int& maxRoomWidth, const int& maxRoomHeight) {
            maxWidth = maxRoomWidth;
            maxHeight = maxRoomHeight;
            setupRoom();
        }

        Room(const int& maxRoomWidth, const int& maxRoomHeight, const char& entranceWall, Room* prevRoom) {
            hasPrevRoom = true;
            maxWidth = maxRoomWidth;
            maxHeight = maxRoomHeight;
            setupRoom(entranceWall);
            // 0 is guaranteed to be the entrance index after setup
            exitRoomsMap[0] = prevRoom;
        }

        ~Room() {
            // free all neighbour rooms not including potential parent room at index 0
            int n = (hasPrevRoom) ? 1 : 0;
            for (; n < exits.size(); n++) {
                if (exitRoomsMap[n] != NULL) {
                    delete exitRoomsMap[n];
                }
            }
        }

        const std::vector<std::vector<char>> getMap() {
            return map;
        }

        Room* update(double dt) {
            // -- input --
            getInput();

            // -- update --
            player.update(dt, map, wallSize);

            // if a player is on an exit, enter new room
            // get pointer to neighbour room corresponding to exit and add new room if required
            Point2D mapNormalisedPlayer((int) player.x() / wallSize, (int) player.y() / wallSize);
            std::unordered_map<char, char> exitEntranceWallMap = {{'l', 'r'}, {'r', 'l'}, {'t', 'b'}, {'b', 't'}}; // exit: entrance
            int exitIndex;
            if ((exitIndex = getValueIndex(exits, mapNormalisedPlayer)) >= 0) {
                // add new room if none already associated with exit
                if (exitRoomsMap[exitIndex] == NULL) {
                    char newRoomEntranceWall = exitEntranceWallMap[exitWallMap[exits[exitIndex]]];
                    exitRoomsMap[exitIndex] = new Room(maxWidth, maxHeight, newRoomEntranceWall, this);
                }
                return exitRoomsMap[exitIndex];
            }

            return this;
        }

        // determine whether point that conforms to map grid is in room
        bool normalisedPointInRoom(const Point2D& p) {
            return p.x() >= 0 && p.y() >= 0 && p.x() < width && p.y() < height;
        }

        // determine whether point that does NOT conform to map grid is in room
        bool pointInRoom(const Point2D& p) {
            // normalise coords to map grid
            double x = p.x() / wallSize;
            double y = p.y() / wallSize;
            return x >= 0 && y >= 0 && x < width && y < height;
        }

        void draw(Window& window) {
            if (drawMode2D) {
                draw2D(window);
            } else {
                draw3D(window);
            }
        }

    private:
        Random random;
        static inline bool drawMode2D = true;
        const int maxDof = 8;

        int maxWidth;
        int maxHeight;
        int width;
        int height;

        bool hasPrevRoom = false;  // used for destructor to prevent freeing parent room

        // need to store rooms so they arent freed once update stackframe pops
        std::vector<Point2D> exits;
        std::vector<std::vector<char>> map;
        std::unordered_map<int, Room*> exitRoomsMap;  // exit index: room pointer
        std::unordered_map<Point2D, char, PointHasher> exitWallMap;  // exit: wall tblr
        Player player;

        Point2D randomPointOnWall(const char& wall) {
            // exclude corners
            int randomHeight = random.between(1, height - 1);
            int randomWidth = random.between(1, width - 1);

            if (wall == 'l') {
                return Point2D(0, randomHeight);
            } else if (wall == 'r') {
                return Point2D(width - 1, randomHeight);
            } else if (wall == 't') {
                return Point2D(randomWidth, 0);
            } else if (wall == 'b') {
                return Point2D(randomWidth, height - 1);
            } else {
                return Point2D();
            }
        }

        // get map made for room
        void setupRoom(const char& entranceWall='n') {
            // -- dimension constraints --
            if (maxWidth < 3) {
                maxWidth = 3;
            }
            if (maxHeight < 3) {
                maxHeight = 3;
            }

            do {
                generateRoom(entranceWall);
            } while (!roomTraversable());
            // convert from grid coord space to window coord space
            player.set(player.x() * wallSize, player.y() * wallSize);
        }

        // generate a random, potentially invalid room layout
        void generateRoom(const char& entranceWall='n') {
            map.clear();
            exits.clear();
            width = random.between(3, maxWidth);
            height = random.between(3, maxHeight);

            // -- create random map --
            for (int y = 0; y < height; y++) {
                map.push_back(std::vector<char>());
                for (int x = 0; x < width; x++) {
                    // create walls
                    if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                        map[y].push_back(WALL);
                    // create random internal walls
                    } else {
                        map[y].push_back((random.random(4) == 0) ? WALL : EMPTY);
                    }
                }
            }

            // -- fill in holes --
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < height; x++) {
                    Point2D p = Point2D(x, y);
                    int wallNeighbours = 0;
                    for (auto n : p.getCardinalNeighbours()) {
                        if (normalisedPointInRoom(n) && map[n.y()][n.x()] == WALL) {
                            wallNeighbours++;
                        }
                    }
                    if (wallNeighbours >= 3) {
                        map[p.y()][p.x()] = WALL;
                    }
                }
            }

            // -- generate entrance and set player spawn --
            // player is generated in map coordinate space for generation
            // then will later be converted to screen pixel coordinate space
            if (entranceWall == 'n') {
                player.set(1, 1);
            } else {
                Point2D entrance = randomPointOnWall(entranceWall);
                player.set(entrance);
                exits.push_back(entrance);
                exitWallMap[entrance] = entranceWall;
            }
            map[player.y()][player.x()] = EMPTY;

            // -- generate exits --
            int numExits = random.random(3) + 1;  // must be at least one exit
            std::vector<char> exitDirections = {'l', 'r', 't', 'b'};
            // remove entrance wall
            if (contains(exitDirections, entranceWall)) {
                popAllOfValue(exitDirections, entranceWall);
            }
            // generate other exit(s)
            for (int exit = 0; exit < numExits; exit++) {
                int exitTypeIndex = random.random(exitDirections.size());
                char wall = exitDirections[exitTypeIndex];
                popAllOfValue(exitDirections, wall);  // remove to avoid duplicates
                Point2D exitPoint = randomPointOnWall(wall);
                map[exitPoint.y()][exitPoint.x()] = EMPTY;  // update map
                exits.push_back(exitPoint);
                exitWallMap[exitPoint] = wall;
            }
        }

        // verify room layout is traversable with bfs to exits
        bool roomTraversable() {
            std::queue<Point2D> visitQueue;
            std::unordered_map<Point2D, bool, PointHasher> visited;
            int visitedExits = 0;

            visitQueue.push(player);
            visited[player] = true;
            while (!visitQueue.empty() && visitedExits != exits.size()) {
                Point2D p = visitQueue.front();
                if (contains(exits, p)) {
                    visitedExits++;
                }

                for (auto n : p.getCardinalNeighbours()) {
                    if (normalisedPointInRoom(n) && map[n.y()][n.x()] == EMPTY && !visited[n]) {
                        visitQueue.push(n);
                        visited[n] = true;
                    }
                }

                visitQueue.pop();
            }

            if (visitedExits == exits.size()) {
                return true;
            }
            return false;
        }
    
        void getInput() {
            std::unordered_map<SDL_Keycode, bool> keydowns = Input::getKeydowns();

            if (keydowns[SDLK_1]) {
                drawMode2D = !drawMode2D;
            }
        }

        void draw2D(Window& window) {
            SDL_Rect tile = {0, 0, wallSize, wallSize};
            for (int y = 0; y < map.size(); y++) {
                tile.y = y * wallSize;
                for (int x = 0; x < map[y].size(); x++) {
                    tile.x = x * wallSize;
                    if (map[y][x] == WALL) {
                        window.renderRect(tile, Colours::grey);
                    }
                }
            }

            player.draw2D(window);
            
            // debug raycasts
            double fovRad = player.getFovRad();
            for (double r = fovRad/2; r > -fovRad/2; r = r - fovRad/window.screenWidth) {
                double angle = player.getRotRad() + r;
                Ray2D ray = Ray2D(player, angle, maxDof, wallSize, map);
                if (ray.getHit()) {
                    window.renderLine(player, ray.getHitPos(), Colours::magenta);
                } else {
                    window.renderLine(player, ray.getHitPos(), Colours::yellow);
                }
            }
        }

        void draw3D(Window& window) {
            int x = 0;  // increment accross screen
            int w = 1;  // one slice every pixel
            int y, h;

            double fovRad = player.getFovRad();
            for (double r = fovRad/2; r > -fovRad/2; r = r - fovRad/window.screenWidth) {
                double rayAngle = player.getRotRad() + r;
                Ray2D ray = Ray2D(player, rayAngle, maxDof, wallSize, map);
                double rayLength = ray.getLength();

                // correct fisheye
                double angleDifference = wrapRadAngle(player.getRotRad() - rayAngle);
                rayLength *= cos(angleDifference);
                
                h = wallSize * window.screenHeight / rayLength;
                y = (window.screenHeight - h) / 2;
                SDL_Rect slice = {x, y, w, h};
                int value = (int) (255 * window.screenHeight / rayLength / wallSize);
                // value adjustments and capping
                value += 30;
                if (value > 200) { value = 200; }
                if (ray.getHitAxis() == RayHitAxis::horizontal) { value -= 20; }
                if (value < 0) { value = 0; }
                Colour colour = {value, value, value, value};

                if (ray.getHit()) {
                    window.renderRect(slice, colour);
                }
                x++;
            }
        }
};
