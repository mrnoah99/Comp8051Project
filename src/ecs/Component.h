#pragma once
#include <string>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <functional>

#include "../utils/Vector2D.h"
#include "system/AnimationClip.h"


struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    Vector2D prevPosition{};
};

struct Velocity {
    Vector2D direction{};
    float speed{};
    Vector2D previousMoveDirection{};
};

struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

enum class RenderLayer {
    WORLD,
    UI
};

struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
    RenderLayer layer = RenderLayer::WORLD;
    bool visible = true;
};

struct Collider {
    std::string tag;
    SDL_FRect rect{};
    bool enabled = true;
};

struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{};
    int currentFrame{};
    float speed = 0.1f;
};

struct Camera {
    SDL_FRect view;
    float worldW;
    float worldH;
};

struct TimedSpawner {
    float spawnInterval{};
    std::function<void()> spawnCallback{};
    float timer{};
};

struct SceneState {
    int coinsCollected{};
};

struct Health {
    int currentHealth{};
    int maxHealth{};
};

struct Clickable {
    std::function<void()> onPressed{};
    std::function<void()> onReleased{};
    std::function<void()> onCancel{};
    bool pressed = false;
};

struct Parent {
    Entity* parent = nullptr;
};

struct Children {
    std::vector<Entity*> children;
};

struct PlayerTag{};
struct ProjectileTag{};