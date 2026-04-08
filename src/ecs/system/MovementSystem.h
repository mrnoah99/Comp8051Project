#include <memory>
#include <vector>
#include <cmath>

#include "../Entity.h"
#include "../Component.h"

class MovementSystem {
    private:
        int worldW{}, worldH{};

    public:
        void update(std::vector<std::unique_ptr<Entity>>& entities, float deltaTime) {
            for (auto& entity : entities) {
                if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
                    auto& transform = entity->getComponent<Transform>();
                    auto& vel = entity->getComponent<Velocity>();

                    // track previous frame position
                    transform.prevPosition = transform.position;

                    // get direction (updated by rotation system)
                    Vector2D directionVec = vel.direction;
                    directionVec.normalize();
                    Vector2D velocity = directionVec * vel.speed;

                    // apply position change
                    transform.position += (velocity * deltaTime);

                    // clamp position to world bounds
                    if (transform.position.x > worldW) transform.position.x = worldW;
                    if (transform.position.y > worldH) transform.position.y = worldH;

                    if (transform.position.x < 0) transform.position.x = 0;
                    if (transform.position.y < 0) transform.position.y = 0;
                }
            }
        }

        void updateWorldSize(int w, int h) {
            worldW = w; worldH = h;
        }
};