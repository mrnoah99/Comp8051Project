#include <vector>
#include <memory>

#include "../Component.h"
#include "../Entity.h"
#include "../../utils/RenderUtils.h"
#include "../../manager/TextureManager.h"

class UIRenderSystem {
    public:
        void render(const std::vector<std::unique_ptr<Entity>>& entities) {
            for (auto& e : entities) {
                if (e->hasComponent<Transform>() && e->hasComponent<Sprite>()) {
                    auto t = e->getComponent<Transform>();
                    auto s = e->getComponent<Sprite>();

                    if (s.layer != RenderLayer::UI) continue;

                    s.dst.x = t.position.x;
                    s.dst.y = t.position.y;

                    if (s.visible) {
                        SDL_FRect scaledDst = RenderUtils::getScaledDest(s.dst, t.scale);
                        TextureManager::draw(s.texture, s.src, scaledDst);
                    }
                }
            }
        }
};