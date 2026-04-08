#include "Game.h"
#include "manager/AssetManager.h"

GameState Game::gameState{};

std::function<void(std::string)> Game::onSceneChangeRequest;

Game::Game() {}

Game::~Game() {
    destroy();
}

void Game::init(const char* title, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) flags = SDL_WINDOW_FULLSCREEN;

    if (SDL_Init(SDL_INIT_VIDEO) == 1) {
        window = SDL_CreateWindow(title, width, height, flags);

        if (!window) {isRunning = false; return;}

        renderer = SDL_CreateRenderer(window, nullptr);

        if (!renderer) {isRunning = false; return;}

        sceneManager = std::make_unique<SceneManager>();

        if (!sceneManager) {isRunning = false; return;}

        // sceneManager->loadScene(SceneType::Gameplay, "level1", "./assets/maps/level-1.tmx", 1280, 720);
        sceneManager->loadScene(SceneType::Gameplay, "level2", "./assets/maps/level-2.tmx", 1280, 720);
        sceneManager->changeSceneDeferred("level2");

        isRunning = true;
    } else {
        isRunning = false;
    }

    Game::gameState = {3, 3, 3, 3, 12};
}

void Game::handleEvents() {
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_EVENT_QUIT:
            isRunning = false;
            break;
        default:
            break;
    }
}

void Game::update(float deltaTime) {
    frameCount++;

    sceneManager->update(deltaTime, event);
}

void Game::render() {
    SDL_RenderClear(renderer);
    sceneManager->render();
    SDL_RenderPresent(renderer);
}

void Game::destroy() {
    TextureManager::clean();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}