#pragma once

#include <iostream>
#include <unordered_map>
#include <string>

#include "../include/SDL2/SDL.h"

class Input {
    public:
    // MARK: -- COMMAND LINE ---------------------------------------------
        static char getKey() {
            std::string line;
            std::cin >> line;
            if (line.size() > 0) {
                return line[0];
            }
            return '\0';
        }

    // MARK: -- SDL -------------------------------------------------------
        static const bool isQuit() {
            return quit;
        }

        static const std::unordered_map<SDL_Keycode, bool> getPressed() {
            return pressed;
        }

        static const std::unordered_map<SDL_Keycode, bool> getKeydowns() {
            return keydowns;
        }

        static const std::unordered_map<SDL_Keycode, bool> getKeyups() {
            return keyups;
        }

        static void readEvents() {
            keydowns.clear();
            keyups.clear();
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN) {
                    pressed[e.key.keysym.sym] = true;
                    keydowns[e.key.keysym.sym] = true;
                } else if (e.type == SDL_KEYUP) {
                    pressed[e.key.keysym.sym] = false;
                    keyups[e.key.keysym.sym] = true;
                }
            }
        }

    private:
        static inline std::unordered_map<SDL_Keycode, bool> pressed;  // state of all keys
        static inline std::unordered_map<SDL_Keycode, bool> keydowns; // key downs at last read events call
        static inline std::unordered_map<SDL_Keycode, bool> keyups;   // key ups at last read events call
        static inline bool quit = false;
};