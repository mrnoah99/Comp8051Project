#include <vector>
#include <memory>

#include "../Entity.h"
#include "../Component.h"

class CameraSystem {
    public:
        void update(const std::vector<std::unique_ptr<Entity>>& entities) {
            Entity* playerEnt = nullptr;

            for (auto& e : entities) {
                if (e->hasComponent<PlayerTag>()) {
                    playerEnt = e.get();
                    break;
                }
            }

            if (!playerEnt) return;

            auto& playerTransform = playerEnt->getComponent<Transform>();

            for (auto& e : entities) {
                if (e->hasComponent<Camera>()) {
                    auto& cam = e->getComponent<Camera>();

                    cam.view.x = playerTransform.position.x - cam.view.w / 2;
                    cam.view.y = playerTransform.position.y - cam.view.h / 2;

                    if (cam.view.x < 0) {
                        cam.view.x = 0;
                    }

                    if (cam.view.y < 0) {
                        cam.view.y = 0;
                    }

                    if (cam.view.x > cam.worldW - cam.view.w) {
                        cam.view.x = cam.worldW - cam.view.w;
                    }

                    if (cam.view.y > cam.worldH - cam.view.h) {
                        cam.view.y = cam.worldH - cam.view.h;
                    }
                }
            }
        }
};