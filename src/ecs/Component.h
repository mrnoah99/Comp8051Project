#pragma once
#include <string>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <unordered_map>
#include <functional>

#include "../utils/Vector2D.h"
#include "system/AnimationClip.h"
#include "Entity.h"

// tracks position, rotation, and scale
struct Transform {
    Vector2D position{};
    float rotation{};
    float rotationSpeed{};
    float scale{};
    Vector2D prevPosition{};
    float rotationForceLeft{};
};

// tracks speed and direction
struct Velocity {
    Vector2D direction{};
    float speed{};
    float maxSpeed{};
    float acceleration{};
    Vector2D previousMoveDirection{};
};

// Classifies entities between world and menus/other UI
enum class RenderLayer {
    WORLD,
    UI
};

// displays image
struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
    RenderLayer layer = RenderLayer::WORLD;
    bool visible = true;
};

// tracks collisions
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

// animates entities with multiple sprites
struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{};
    int currentFrame{};
    float speed = 0.1f;
};

// displays the game view
struct Camera {
    SDL_FRect view;
    float worldW;
    float worldH;
};

// spawns entities over time
struct TimedSpawner {
    float spawnInterval{};
    std::function<void()> spawnCallback{};
    float timer{};
};

// tracks items collected
struct SceneState {
    int coinsCollected{};
};

// tracks health values
struct Health {
    int currentHealth{};
    int maxHealth{};
};

// tracks health for a vehicle part
struct VehiclePartHealth {
    int health{};
    int maxHealth{};
    std::function<void(VehiclePartHealth& part, int n)> updateHealth{};
};

// front facing vehicle part
struct Engine {
    VehiclePartHealth partHealth{};
};

// rear facing vehicle part
struct FuelTransmission {
    VehiclePartHealth partHealth{};
};

// left facing vehicle part
struct LeftWheels {
    VehiclePartHealth partHealth{};
};

// right facing vehicle part
struct RightWheels {
    VehiclePartHealth partHealth{};
};

// menu button
struct Clickable {
    std::function<void()> onPressed{};
    std::function<void()> onReleased{};
    std::function<void()> onCancel{};
    bool pressed = false;
};

// parent-child relationship
struct Parent {
    Entity* parent = nullptr;
};

// parent-child relationship
struct Children {
    std::vector<Entity*> children;
};

// used to import camera location data from map if applicable
struct CameraLocation {
    SDL_FRect camera;
};

// tags
struct PlayerTag{};
struct Player2Tag{};
struct FinishLineTag{};