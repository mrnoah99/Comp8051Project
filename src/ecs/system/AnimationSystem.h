#include <vector>
#include <memory>
#include <string>

#include "../Entity.h"
#include "../Component.h"

class AnimationSystem {
    public:
        void update(const std::vector<std::unique_ptr<Entity>>& entities, float deltaTime) {
            for (auto& e : entities) {
                if (e->hasComponent<Animation>() && e->hasComponent<Velocity>()) {
                    auto& anim = e->getComponent<Animation>();
                    auto& vel = e->getComponent<Velocity>();

                    //state system
                    std::string newClip;
                    if (vel.direction.x > 0.0f) {
                        newClip = "walk_right";
                        vel.previousMoveDirection.x = 1;
                        vel.previousMoveDirection.y = 0;
                    } else if (vel.direction.x < 0.0f) {
                        newClip = "walk_left";
                        vel.previousMoveDirection.x = -1;
                        vel.previousMoveDirection.y = 0;
                    } else if (vel.direction.y < 0.0f) {
                        newClip = "walk_up";
                        vel.previousMoveDirection.y = 1;
                        vel.previousMoveDirection.x = 0;
                    } else if (vel.direction.y > 0.0f) {
                        newClip = "walk_down";
                        vel.previousMoveDirection.y = -1;
                        vel.previousMoveDirection.x = 0;
                    } else if (vel.previousMoveDirection.x > 0.0f) {
                        newClip = "idle_right";
                    } else if (vel.previousMoveDirection.x < 0.0f) {
                        newClip = "idle_left";
                    } else if (vel.previousMoveDirection.y > 0.0f) {
                        newClip = "idle_up";
                    } else if (vel.previousMoveDirection.y < 0.0f) {
                        newClip = "idle_down";
                    } else {
                        newClip = "idle_right";
                    }

                    //check if anim has switched
                    if (newClip != anim.currentClip) {
                        anim.currentClip = newClip;
                        anim.time = 0.0f;
                        anim.currentFrame = 0;
                    }

                    //playback system
                    float frameSpeed = anim.speed;
                    auto it = anim.clips.find(anim.currentClip);
                    if (it == anim.clips.end()) continue;
                    auto& clip = it->second;
                    if (clip.frameIndices.empty()) continue;
                    anim.time += deltaTime;

                    if (anim.time >= frameSpeed) {
                        anim.time -= frameSpeed;

                        std::size_t totalFrames = clip.frameIndices.size();
                        anim.currentFrame = (anim.currentFrame + 1) % (totalFrames > 0? totalFrames : 1);
                    }
                }
            }
        }
};