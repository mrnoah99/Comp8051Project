#pragma once

#include <functional>

#include "../event/BaseEvent.h"

class Entity;
class World;

class EventResponseSystem {
    public:
        EventResponseSystem(World &world);
    private:
        void onCollision(const CollisionEvent& e, const char* otherTag, World& world);
        bool getCollisionEntities(const CollisionEvent& e, const char* otherTag, Entity *&player, Entity *&other);

        void onPlayerAction(const PlayerActionEvent& e, const std::function<void(Entity* player, PlayerAction action)>& callback);
        void onMouseInteraction(const MouseInteractionEvent& e);
    };