#pragma once

class Entity;

enum class EventType {
    Collision,
    PlayerAction,
    MouseInteraction
};

struct BaseEvent {
    EventType type{};
    virtual ~BaseEvent() = default;
};

enum class CollisionState {
    Enter, Stay, Exit
};

struct CollisionEvent : BaseEvent {
    Entity* entA = nullptr;
    Entity* entB = nullptr;
    CollisionState state{};
    CollisionEvent(Entity* a, Entity* b, CollisionState s) : entA(a), entB(b), state(s) {
        type = EventType::Collision;
    }
};

enum class PlayerAction {
    Attack,
    Interact,
    UseItem,
    Jump
};

struct PlayerActionEvent : BaseEvent {
    Entity* player = nullptr;
    PlayerAction action{};
    PlayerActionEvent(Entity* p, PlayerAction a) : player(p), action(a) {
        type = EventType::PlayerAction;
    }
};

enum class MouseInteractionState {Pressed, Released, Cancel};

struct MouseInteractionEvent : BaseEvent {
    Entity* entity = nullptr;
    MouseInteractionState state{};
    MouseInteractionEvent(Entity* entity, MouseInteractionState state) : entity(entity), state(state) {
        type = EventType::MouseInteraction;
    }
};
