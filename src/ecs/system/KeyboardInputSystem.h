#include <memory>
#include <vector>
#include <SDL3/SDL_events.h>

#include "../Component.h"
#include "../Entity.h"

class KeyboardInputSystem {
    public:
        void update(const std::vector<std::unique_ptr<Entity>>& entities) {
            const bool* keystates = SDL_GetKeyboardState(NULL);

            for (auto& e : entities) {
                if (e->hasComponent<Velocity>() && e->hasComponent<PlayerTag>() && e->hasComponent<Transform>()) {
                    auto& v = e->getComponent<Velocity>();
                    auto& t = e->getComponent<Transform>();

                    if (keystates[SDL_SCANCODE_W]) v.speed += v.acceleration;
                    if (keystates[SDL_SCANCODE_S]) v.speed -= v.acceleration;

                    if (keystates[SDL_SCANCODE_A]) t.rotation -= t.rotationSpeed;
                    if (keystates[SDL_SCANCODE_D]) t.rotation += t.rotationSpeed;
                    
                    if (!keystates[SDL_SCANCODE_W] && !keystates[SDL_SCANCODE_S]) {    
                        // apply drag when not accelerating
                        if (v.speed > 1.0f)
                            v.speed -= 1.0f;

                        if (v.speed < -1.0f)
                            v.speed += 1.0f;

                        if (v.speed < 0.5f && v.speed > -0.5f)
                            v.speed = 0.0f;
                    }

                    // clamp speed and rotation
                    if (v.speed >= v.maxSpeed) v.speed = v.maxSpeed;
                    if (v.speed <= v.maxSpeed*-0.5f) v.speed = v.maxSpeed*-0.5f;
                    if (t.rotation > 360) t.rotation = 0;
                    if (t.rotation < 0) t.rotation = 360;
                }

                if (e->hasComponent<Velocity>() && e->hasComponent<Player2Tag>() && e->hasComponent<Transform>()) {
                    auto& v = e->getComponent<Velocity>();
                    auto& t = e->getComponent<Transform>();

                    if (keystates[SDL_SCANCODE_UP]) v.speed += v.acceleration;
                    if (keystates[SDL_SCANCODE_DOWN]) v.speed -= v.acceleration;

                    if (keystates[SDL_SCANCODE_LEFT]) t.rotation -= t.rotationSpeed;
                    if (keystates[SDL_SCANCODE_RIGHT]) t.rotation += t.rotationSpeed;
                    
                    if (!keystates[SDL_SCANCODE_UP] && !keystates[SDL_SCANCODE_DOWN]) {    
                        // apply drag when not accelerating
                        if (v.speed > 1.0f)
                            v.speed -= 1.0f;

                        if (v.speed < -1.0f)
                            v.speed += 1.0f;

                        if (v.speed < 0.5f && v.speed > -0.5f)
                            v.speed = 0.0f;
                    }

                    // clamp speed and rotation
                    if (v.speed >= v.maxSpeed) v.speed = v.maxSpeed;
                    if (v.speed <= v.maxSpeed*-0.5f) v.speed = v.maxSpeed*-0.5f;
                    if (t.rotation > 360) t.rotation = 0;
                    if (t.rotation < 0) t.rotation = 360;
                }
            }
        }
};