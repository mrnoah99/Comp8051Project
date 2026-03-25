#pragma once
#include <SDL3/SDL_events.h>
#include <string>

#include "../ecs/World.h"
#include "SceneType.h"
#include "../ecs/Component.h"

class Scene {
    public:
        Scene(SceneType type, const std::string& sceneName, const std::string& mapPath, const int windowWidth, const int windowHeight);

        void update(const float deltaTime, const SDL_Event &event) {
            world.update(deltaTime, event, type);
        }

        void render() {
            world.render();
        }

        World world;

        const std::string getName() {
            return name;
        }

    private:
        std::string name;
        SceneType type;
        void createProjectile(Vector2D pos, Vector2D dir, int speed);

        void initMainMenu(int windowWidth, int windowHeight);
        void initGameplay(const char* mapPath, int windowWidth, int windowHeight);

        Entity& createSettingsOverlay(int windowWidth, int windowHeight);
        Entity& createCogButton(int windowWidth, int windowHeight, Entity& overlay);
        void createSettingsUIComponents(Entity& overlay);
        void toggleSettingsOverlayVisibility(Entity& overlay);

};