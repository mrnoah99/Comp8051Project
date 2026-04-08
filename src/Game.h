#pragma once
#include <SDL3/SDL.h>

#include "ecs/World.h"
#include "manager/SceneManager.h"

struct GameState {
    int engine1Health{};
    int rWheel1Health{};
    int lWheel1Health{};
    int transmission1Health{};
    int player1Health{};

    int engine2Health{};
    int rWheel2Health{};
    int lWheel2Health{};
    int transmission2Health{};
    int player2Health{};

    int p1Laps = 0;
    int p2Laps = 0;
};

class Game {
    public:
        Game();
        ~Game();

        static bool player2;

        // Same types as SDL uses
        void init(const char* title, int width, int height, bool fullscreen);

        // game loop events
        void handleEvents(); // checks for input

        void update(float deltaTime); // handles game logic and changes to game state

        void render(); // draws to the screen

        void destroy(); // frees resources

        bool running() {
            return isRunning;
        }

        SDL_Renderer* renderer = nullptr;

        std::unique_ptr<SceneManager> sceneManager;

        static std::function<void(std::string)> onSceneChangeRequest;
        static GameState gameState;

    private:
        int frameCount = 0;
        bool isRunning = false;
        bool sceneChangeComplete = false;

        SDL_Window* window = nullptr;

        SDL_Event event;
};