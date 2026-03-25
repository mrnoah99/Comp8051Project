#include <iostream>
#include "Game.h"
#include <SDL3/SDL.h>

//global variable
Game *game = nullptr;

int main() {
    const int FPS = 60; // refresh rate of most monitors
    const int desiredFrameTime = 1000/FPS; //16ms per frame 

    Uint64 ticks;
    float deltaTime = 0.0f;

    int actualFrameTime;

    game = new Game();
    game->init("8051 Project", 1280, 720, false);

    while (game->running()) {
        int currentTicks = SDL_GetTicks(); // time in ms since SDL was initialized
        deltaTime = (currentTicks - ticks) / 1000.0f;
        ticks = currentTicks;
        
        game->handleEvents();
        game->update(deltaTime);
        game->render();

        actualFrameTime = SDL_GetTicks() - ticks; // actual elapsed time in ms for the frame

        //limit frames
        if (desiredFrameTime > actualFrameTime) {
            SDL_Delay(desiredFrameTime - actualFrameTime);
        }
    }

    delete game;

    return 0;
}