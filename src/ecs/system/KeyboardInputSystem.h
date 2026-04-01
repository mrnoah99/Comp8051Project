#include <memory>
#include <vector>
#include <SDL3/SDL_events.h>

#include "../Component.h"
#include "../Entity.h"

class KeyboardInputSystem {
    public:
        void update(const std::vector<std::unique_ptr<Entity>>& entities, SDL_Event& event) {
            for (auto& e : entities) {
                if (e->hasComponent<Velocity>() && e->hasComponent<PlayerTag>()) {
                    auto& v = e->getComponent<Velocity>();
                    auto& t = e->getComponent<Transform>();
                    if (event.type == SDL_EVENT_KEY_DOWN) {
                        switch (event.key.key) {
                            case SDLK_W:
                                v.speed += v.acceleration;
                                break;
                            case SDLK_S:
                                v.speed -= v.acceleration;
                                break;
                            case SDLK_A:
                                v.direction.x = -1;
                                t.rotation -= 1;
                                break;
                            case SDLK_D:
                                v.direction.x = 1;
                                t.rotation += 1;
                                break;
                            default:
                                break;
                        }
                    }
                    if (v.speed >= v.maxSpeed) v.speed = v.maxSpeed;
                    if (v.speed <= v.maxSpeed*-1) v.speed = v.maxSpeed*-1;
                    if (t.rotation > 360) t.rotation = 0;
                    if (t.rotation < 0) t.rotation = 360;
                    if (event.type == SDL_EVENT_KEY_UP) {
                        switch (event.key.key) {
                            case SDLK_W:
                                v.direction.y = 0;
                                break;
                            case SDLK_S:
                                v.direction.y = 0;
                                break;
                            case SDLK_A:
                                v.direction.x = 0;
                                break;
                            case SDLK_D:
                                v.direction.x = 0;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
};