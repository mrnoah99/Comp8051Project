#include <memory>
#include <vector>

#include "../Component.h"
#include "../Entity.h"
#include "../../manager/TextureManager.h"

class RenderSystem {
    public:
        void render(std::vector<std::unique_ptr<Entity>>& entities) {
            Entity* cameraEnt = nullptr;

            for (auto& e : entities) {
                if (e->hasComponent<Camera>()) {
                    cameraEnt = e.get();
                    break;
                }
            }

            if (!cameraEnt) return;
            auto& cam = cameraEnt->getComponent<Camera>();

            for (auto& entity : entities) {
                if (entity->hasComponent<Transform>() && entity->hasComponent<Sprite>()) {
                    auto& transform = entity->getComponent<Transform>();
                    auto& sprite = entity->getComponent<Sprite>();
                    
                    if (sprite.layer != RenderLayer::WORLD) continue;

                    sprite.dst.x = transform.position.x - cam.view.x;
                    sprite.dst.y = transform.position.y - cam.view.y;

                    if (entity->hasComponent<Animation>()) {
                        auto& anim = entity->getComponent<Animation>();

                        auto it = anim.clips.find(anim.currentClip);
                        if (it != anim.clips.end()) {
                            auto& clip = it->second;

                            if (!clip.frameIndices.empty() &&
                                anim.currentFrame < (int) clip.frameIndices.size()) {

                                sprite.src = clip.frameIndices[anim.currentFrame];
                            }
                        }
                    }

                    TextureManager::draw(sprite.texture, sprite.src, sprite.dst);
                }
            }
        }
};