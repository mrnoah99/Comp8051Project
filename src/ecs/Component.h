#pragma once
#include <string>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <functional>

#include "../utils/Vector2D.h"
#include "system/AnimationClip.h"
#include "Entity.h"


struct Transform {
    Vector2D position{};
    float rotation{};
    float rotationSpeed{};
    float scale{};
    Vector2D prevPosition{};
    float rotationForceLeft{};
};

struct Velocity {
    Vector2D direction{};
    float speed{};
    float maxSpeed{};
    float acceleration{};
    Vector2D previousMoveDirection{};
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
    Vector2D centre{};
    Vector2D halfSize{};
    Vector2D right{};
    Vector2D forward{};
    float rotation{};
    float collisionRotationFactor{};
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

struct VehiclePartHealth {
    int health{};
    int maxHealth{};
    std::function<void(VehiclePartHealth& part, int n)> updateHealth{};
};

struct Engine {
    VehiclePartHealth partHealth{};
};

struct FuelTransmission {
    VehiclePartHealth partHealth{};
};

struct LeftWheels {
    VehiclePartHealth partHealth{};
};

struct RightWheels {
    VehiclePartHealth partHealth{};
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
struct Player2Tag{};
struct CameraLocation {
    SDL_FRect camera;
};
struct FinishLineTag{};