#include <string>
#include <unordered_map>
#include <memory>

#include "../scene/Scene.h"

struct SceneParams {
    SceneType type;
    std::string name;
    std::string mapPath;
    int windowWidth;
    int windowHeight;
};

class SceneManager {
    std::unordered_map<std::string, SceneParams> sceneParams;
    std::string pendingScene;

    void changeScene(const std::string& name) {
        auto it = sceneParams.find(name);
        if (it != sceneParams.end()) {
            const auto& params = it->second;
            currentScene = std::make_unique<Scene>(
                params.type,
                params.name,
                params.mapPath,
                params.windowWidth,
                params.windowHeight
            );
        }
    }

    public:
        std::unique_ptr<Scene> currentScene;
        void loadScene(SceneType sceneType, const std::string sceneName, const std::string mapPath, const int windowWidth, const int windowHeight) {
            sceneParams[sceneName] = {sceneType, sceneName, mapPath, windowWidth, windowHeight};
        }

        void changeSceneDeferred(const std::string& name) {
            pendingScene = name;
        }

        void update(const float deltaTime, SDL_Event event) {
            if (currentScene) currentScene->update(deltaTime, event);
            if (!pendingScene.empty()) {
                changeScene(pendingScene);
                pendingScene.clear();
            }
        }

        void render() const {
            if (currentScene) currentScene->render();
        }
};