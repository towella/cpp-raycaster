#include <iostream>
#include <unordered_map>
#include <string>

#include "../include/SDL2/SDL.h"
#include "../headers/window.hpp"
#include "../headers/input.hpp"
#include "../headers/point.hpp"
#include "../headers/room.hpp"

int main(int argc, char* argv[]) {
    Window window = Window();

    bool run = true;
    // while (run) {
    //     Input::readEvents();  // ensures events are updated globally every frame, need only be called here
    //     std::unordered_map<SDL_Keycode, bool> press = Input::getPressed();
    //     run = !Input::isQuit() && !press[SDLK_COMMA];

    //     sdl.clear();
    //     sdl.renderRect(0, 0, 50, 50, 255, 255, 255, 255);
    //     sdl.presentRender();
    // }

    Room firstRoom = Room(20, 20);
    Room* currentRoom = &firstRoom;
    (*currentRoom).draw(window);
    run = true;
    while (run) {
        Input::readEvents();
        std::unordered_map<SDL_Keycode, bool> press = Input::getPressed();
        run = !Input::isQuit() && !press[SDLK_COMMA];

        window.clear();
        currentRoom = (*currentRoom).update();
        (*currentRoom).draw(window);
        window.presentRender();
    }

    window.close();
    return 0;
}