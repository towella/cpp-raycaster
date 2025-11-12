#include <iostream>
#include <unordered_map>
#include <string>

#include "../include/SDL2/SDL.h"
#include "window.hpp"
#include "input.hpp"
#include "room.hpp"

int main(int argc, char* argv[]) {
    const int targetFps = 60;  // SDL auto caps at 60
    const int ticksPerFrame = 1000 / targetFps;  // a tick is a ms
    Window window = Window();

    Room firstRoom = Room(20, 20);
    Room* currentRoom = &firstRoom;
    (*currentRoom).draw(window);

    Uint64 frameTimer = SDL_GetTicks64();
    double dt = 1.0;
    bool run = true;
    while (run) {
        // get delta time
        dt = (double) (SDL_GetTicks64() - frameTimer) * targetFps / 1000;
        frameTimer = SDL_GetTicks64();

        // read input once to be accessed by any system
        Input::readEvents(window);
        std::unordered_map<SDL_Keycode, bool> press = Input::getPressed();
        run = !Input::getQuit() && !press[SDLK_COMMA];

        // update
        currentRoom = (*currentRoom).update(dt);

        // render
        window.clear();
        (*currentRoom).draw(window);
        window.presentRender();

        // delay to cap frame rate
        Uint64 frameTime = SDL_GetTicks64() - frameTimer;
        if (frameTime < ticksPerFrame) {
             SDL_Delay(ticksPerFrame - frameTime);
        }

        // display fps in terminal :)
        // frameTime = SDL_GetTicks64() - frameTimer;
        // std::cout << (double) 1000 / frameTime << '\n';
    }

    window.close();
    return 0;
}