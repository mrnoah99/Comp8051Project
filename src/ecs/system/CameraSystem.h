#include <vector>
#include <memory>

#include "../Entity.h"
#include "../Component.h"

class CameraSystem {
    public:
        void update(const std::vector<std::unique_ptr<Entity>>& entities, bool player2) {
            Entity* playerEnt = nullptr;
            Entity* camLocation = nullptr;

            // get player entity and camera location
            for (auto& e : entities) {
                if (e->hasComponent<PlayerTag>()) {
                    playerEnt = e.get();
                }
                if (e->hasComponent<CameraLocation>()) {
                    camLocation = e.get();
                }
                if (playerEnt && camLocation) break;
            }

            // if no player is found, or player 2 is present, lock camera to provided location
            if (!playerEnt || player2) {
                if (camLocation) {
                    for (auto& e : entities) {
                        if (e->hasComponent<Camera>() && camLocation->hasComponent<CameraLocation>()) {
                            auto& cam = e->getComponent<Camera>();
                            auto& location = camLocation->getComponent<CameraLocation>();

                            cam.view.x = location.camera.x;
                            cam.view.y = location.camera.y;
                            cam.view.w = location.camera.w;
                            cam.view.h = location.camera.h;
                        }
                    }
                }
                return;
            }

            auto& playerTransform = playerEnt->getComponent<Transform>();

            // follow the player
            for (auto& e : entities) {
                if (e->hasComponent<Camera>()) {
                    auto& cam = e->getComponent<Camera>();

                    // centre on player
                    cam.view.x = playerTransform.position.x - (cam.view.w / 2);
                    cam.view.y = playerTransform.position.y - (cam.view.h / 2);

                    // clamp view to map boundary
                    if (cam.view.x < 0) {
                        cam.view.x = 0;
                    }

                    if (cam.view.y < 0) {
                        cam.view.y = 0;
                    }

                    if (cam.view.x > (cam.worldW - cam.view.w)) {
                        cam.view.x = (cam.worldW - cam.view.w);
                    }

                    if (cam.view.y > (cam.worldH - cam.view.h)) {
                        cam.view.y = (cam.worldH - cam.view.h);
                    }
                }
            }
        }
};