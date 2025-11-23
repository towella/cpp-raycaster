#pragma once

#include <iostream>
#include <unordered_map>
#include <string>

#include "../include/SDL2/SDL.h"
#include "window.hpp"

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
        static const bool getQuit() {
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

        static const Point2D getMouse() {
            return mouse;
        }

        static const bool getMouseClicked() {
            return mouseClick;
        }

        static const bool getMouseHeld() {
            return mouseHeld;
        }

        static const bool getMinimised() {
            return minimised;
        }

        static const bool getMouseFocused() {
            return mouseFocused;
        }

        static const bool getKeyboardFocused() {
            return keyboardFocused;
        }

        static void readEvents(Window& window) {
            keydowns.clear();
            keyups.clear();
            mouseClick = false;

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                // -- system events --
                if (e.type == SDL_QUIT) {
                    quit = true;

                // -- keyboard events --
                } else if (e.type == SDL_KEYDOWN) {
                    pressed[e.key.keysym.sym] = true;
                    keydowns[e.key.keysym.sym] = true;
                } else if (e.type == SDL_KEYUP) {
                    pressed[e.key.keysym.sym] = false;
                    keyups[e.key.keysym.sym] = true;
                
                // -- mouse events --
                } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    mouseClick = true;
                    mouseHeld = true;
                } else if (e.type == SDL_MOUSEBUTTONUP) {
                    mouseHeld = false;
                } else if (e.type == SDL_MOUSEMOTION) {
                    updateMousePosition();

                // -- window events --
                } else if (e.type == SDL_WINDOWEVENT) {
                    switch( e.window.event ) {
                        //Get new dimensions and repaint on window size change
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            window.screenWidth = e.window.data1;
                            window.screenHeight = e.window.data2;
                            window.presentRender();
                            break;

                        //Repaint on exposure
                        case SDL_WINDOWEVENT_EXPOSED:
                            window.presentRender();
                            break;

                        //Mouse entered window
                        case SDL_WINDOWEVENT_ENTER:
                            mouseFocused = true;
                            updateMousePosition();
                            break;
                        
                        //Mouse left window
                        case SDL_WINDOWEVENT_LEAVE:
                            mouseFocused = false;
                            mouse.set(-1, -1);
                            break;

                        //Window has keyboard focus
                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            keyboardFocused = true;
                            break;

                        //Window lost keyboard focus
                        case SDL_WINDOWEVENT_FOCUS_LOST:
                            keyboardFocused = false;
                            break;

                        //Window minimized
                        case SDL_WINDOWEVENT_MINIMIZED:
                            minimised = true;
                            break;

                        //Window maximized
                        case SDL_WINDOWEVENT_MAXIMIZED:
                            minimised = false;
                            break;
                        
                        //Window restored from minimised
                        case SDL_WINDOWEVENT_RESTORED:
                            minimised = false;
                            break;
                    }
                }
            }
        }

    private:
        static inline bool quit = false;

        static inline std::unordered_map<SDL_Keycode, bool> pressed;  // state of all keys
        static inline std::unordered_map<SDL_Keycode, bool> keydowns; // key downs at last read events call
        static inline std::unordered_map<SDL_Keycode, bool> keyups;   // key ups at last read events call

        static inline Point2D mouse = Point2D(-1, -1);  // -1, -1 indicates not in window
        static inline bool mouseClick = false;  // clicked this check
        static inline bool mouseHeld = false;  // whether mouse is down irrespective of when clicked

        static inline bool minimised = false;
        static inline bool mouseFocused = false;
        static inline bool keyboardFocused = false;

        static void updateMousePosition() {
            int x, y;
            SDL_GetMouseState( &x, &y );
            mouse.set(x, y);
        }
};