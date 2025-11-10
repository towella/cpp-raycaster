#pragma once

#include <iostream>
#include "../include/SDL2/SDL.h"
#include "point.hpp"

struct Colour {
    int r = 255;
    int g = 255;
    int b = 255;
    int a = 255;
};

// can be accessed with Colours::colourName syntax
struct Colours {
    static inline Colour none = {0, 0, 0, 0};
    static inline Colour red = {255, 0, 0, 255};
    static inline Colour blue = {0, 0, 255, 255};
    static inline Colour green = {0, 255, 0, 255};
    static inline Colour white = {255, 255, 255, 255};
    static inline Colour black = {0, 0, 0, 255};
    static inline Colour grey = {128, 128, 128, 255};
    static inline Colour yellow = {255, 255, 0, 255};
    static inline Colour magenta = {255, 0, 255, 255};
    static inline Colour cyan = {0, 255, 255, 255};
};

class Window {
    public:
        const int screenWidth = 800;
        const int screenHeight = 600;

// MARK: -- SETUP AND SHUTDOWN -------------------------------------------------------

        Window() {
            bool failure = false;

            // 0 indicates success
            if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
                std::cerr << "Error initializing SDL.\n";  // out to standard error stream
                failure = true;
            }

            // create window
            if (!failure) {
                window = SDL_CreateWindow(
                    "My Title",  // pass NULL for no title (like when using flag SDL_WINDOW_BORDERLESS
                    SDL_WINDOWPOS_CENTERED,  // otherwise provide x coord (pos on screen window appears)
                    SDL_WINDOWPOS_CENTERED,  // otherwise provide y coord
                    screenWidth,  // width px
                    screenHeight,  // height px
                    SDL_WINDOW_SHOWN  // window flags (i.e. SDL_WINDOW_BORDERLESS)
                );
                if (!window) {
                    std::cerr << "Error creating SDL window.\n";
                    failure = true;
                }
            }

            // create renderer
            if (!failure) {
                renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                if (!renderer) {
                    std::cerr << "Error creating SDL renderer.\n";
                    failure = true;
                }
                clear();
            }
        }

        void close() {
            // remove in REVERSE order to creation :)
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

// MARK: -- INPUT --------------------------------------------------------------------



// MARK: -- RENDERING ----------------------------------------------------------------

        void clear() {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }

        void renderRect(int x, int y, int w, int h, int r, int g, int b, int a) {
            SDL_Rect rect = {x, y, w, h};
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderFillRect(renderer, &rect);
        }

        void renderRect(const struct SDL_Rect& rect, const Colour& colour=Colours::white) {
            SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
            SDL_RenderFillRect(renderer, &rect);
        }

        void renderLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a) {
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        void renderLine(const Point2D& p1, const Point2D& p2, const Colour& colour=Colours::white) {
            SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
            SDL_RenderDrawLine(renderer, p1.x(), p1.y(), p2.x(), p2.y());
        }

        void presentRender() {
            SDL_RenderPresent(renderer);
        }

    private:
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;
};