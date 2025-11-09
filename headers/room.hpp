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
#include "point2D.hpp"
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
        
        // https://www.youtube.com/watch?v=gYRrGTC7GtA
        std::pair<Point2D, double> rayCast(double rayAngleRad, Point2D origin) {
            // DDA (Digital Differential Analysis)
            // - find offsets
            // - loop through dof using offsets to check intersections with hori or vert grid lines
            // - complete for both axis
            // - use the shorter ray of the two (collided earlier)

            int mapX, mapY;  // ray hit point coordinates normalised to map coords
            double rayX, rayY;  // hitting point of the ray
            double xOffset, yOffset;  // offset to jump to next vert or hori grid line (same every jump)
            double horiX, horiY;  // cache hori ray hit point while computing vert

            double vertRayDist, horiRayDist;  // store length of both rays for comparison
            double infiniteDist = wallSize * maxDof + 1;  // largest distance possible in dof + 1

            double negTan = -tan(rayAngleRad);
            double atan = -1 / tan(rayAngleRad);

            // -- horizontal grid lines check --
            int dof = 0;
            // looking up
            if (rayAngleRad > M_PI/2 && rayAngleRad < 3*M_PI/2) {
                rayY = (((int) origin.y()) / wallSize) * wallSize - 0.0000001;  // snap y to grid (account for float math error)
                rayX = (origin.y() - rayY) * negTan + origin.x();
                yOffset = -wallSize;
                xOffset = -yOffset * negTan;
            // looking down
            } else if (rayAngleRad > 3*M_PI/2 || rayAngleRad < M_PI/2) {
                rayY = (((int) origin.y()) / wallSize) * wallSize + wallSize;  // snap y to grid
                rayX = (origin.y() - rayY) * negTan + origin.x();
                yOffset = wallSize;
                xOffset = -yOffset * negTan;
            // looking straight right or left, skip and use other ray
            } else {
                dof = maxDof;
                horiRayDist = infiniteDist;
            }
            while (dof < maxDof) {
                mapX = (int) rayX / wallSize;
                mapY = (int) rayY / wallSize;
                // has hit
                if (normalisedPointInRoom(Point2D(mapX, mapY)) && map[mapY][mapX] == WALL) {
                    horiRayDist = origin.getDistance(Point2D(rayX, rayY));
                    break;
                // check next horizontal grid line
                } else {
                    dof++;
                    if (dof >= maxDof) {horiRayDist = infiniteDist;}  // has not hit within dof so set dist to max
                    else {
                        rayX += xOffset;
                        rayY += yOffset;
                    }
                }
            }
            horiX = rayX;
            horiY = rayY;

            // -- vertical grid lines check --
            dof = 0;
            // looking left
            if (rayAngleRad > M_PI) {
                rayX = (((int) origin.x()) / wallSize) * wallSize - 0.0000001;  // snap y to grid (account for float math error)
                rayY = (origin.x() - rayX) * atan + origin.y();
                xOffset = -wallSize;
                yOffset = -xOffset * atan;
            // looking right
            } else if (rayAngleRad < M_PI && rayAngleRad != 0) {
                rayX = (((int) origin.x()) / wallSize) * wallSize + wallSize;  // snap y to grid
                rayY = (origin.x() - rayX) * atan + origin.y();
                xOffset = wallSize;
                yOffset = -xOffset * atan;
            // looking straight up or down, skip and use other ray
            } else {
                dof = maxDof;
                vertRayDist = infiniteDist;
            }
            while (dof < maxDof) {
                mapX = (int) rayX / wallSize;
                mapY = (int) rayY / wallSize;
                // has hit
                if (normalisedPointInRoom(Point2D(mapX, mapY)) && map[mapY][mapX] == WALL) {
                    vertRayDist = origin.getDistance(Point2D(rayX, rayY));
                    break;
                // check next horizontal grid line
                } else {
                    dof++;
                    if (dof >= maxDof) {vertRayDist = infiniteDist;} // has not hit within dof so set dist to max
                    else {
                        rayX += xOffset;
                        rayY += yOffset;
                    }
                }
            }

            // return shorter ray (collided with wall earlier)
            if (horiRayDist < vertRayDist) {
                return std::pair(Point2D(horiX, horiY), horiRayDist);
            } else {
                return std::pair(Point2D(rayX, rayY), vertRayDist);
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
            for (double r = fovRad/2; r > -fovRad/2; r = r - M_PI/180) {
                double angle = wrapRadAngle(player.getRotRad() + r);
                window.renderLine(player, rayCast(angle, player).first, Colours::magenta);
            }
        }

        void draw3D(Window& window) {
            int x = 0;  // increment accross screen
            int w = 1;  // one slice every pixel
            int y, h;

            double fovRad = player.getFovRad();
            for (double r = fovRad/2; r > -fovRad/2; r = r - fovRad/window.screenWidth) {
                double angle = wrapRadAngle(player.getRotRad() + r);
                double rayLength = rayCast(angle, player).second;
                
                h = wallSize * window.screenHeight / rayLength;
                y = (window.screenHeight - h) / 2;
                SDL_Rect slice = {x, y, w, h};

                int value = (int) (255 * window.screenHeight / rayLength / wallSize);
                if (value > 200) {
                    value = 200;
                }
                if (rayLength >= wallSize * maxDof) {
                    value = 0;
                }
                Colour colour = {value, value, value, value};
                
                window.renderRect(slice, colour);
                x++;
            }
        }
};
