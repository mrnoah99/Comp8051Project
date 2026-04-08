# Game Engine Overview
This engine is a simple 2D game framework built using SDL3, designed around ECS architecture. It separates most of the engine functionality from game-specific logic, allowing it to be reused for multiple small-scale driving based 2D games.

# Core Architecture: ECS
The engine follows a modern ECS design pattern consisting of:

### Entities
 - Lightweight objects representing game world elements (players, walls, items, camera, etc.)
 - Internally managed as collections of components
 - Stored using std::unique_ptr<Entity> for memory safety

### Components
 - Pure data containers (no logic)
 - Define properties of entities

For example, the Transform component contains position, rotation, and scale information for an Entity. Tag components like PlayerTag differentiate Entities in code so that systems can operate on the correct items.

### Systems
 - Contain all logic
 - Operate only on entities with required components

For example, the MovementSystem handles moving all items with a Transform and Velocity component based on their speed and deltaTime.

### Input
The KeyboardInputSystem uses SDL_GetKeyboardState for real-time input, instead of the event-based system from the assignments.

### Collision
The CollisionSystem uses OBB SAT-based collision detection, which, while a bit buggy, works. It calculates the collision normal, penetration depth, and impact speed.
This applies damage to vehicle components based on the side of the vehicle that collided with something. It also applies some rotation forces and a slight speed reduction because of that when colliding.

### Camera
The camera will track the player (if only one is present) dynamically along the map like from the assignments, but if 2 players are playing, the camera will lock itself into the position provided by the map designer.
Follows map bounds so that it is not pushed out of the world.

### Lifecycle Systems
 - CleanupSystem -> handles tracking entities and deleting them if they die or leave map bounds
 - DestructionSystem -> handles applying penalties to the vehicle when damaged

# Scene Management
### Scenes
Scenes represent individual game levels, and are responsible for:
 - Creating entities
 - Initializing components

For example, Scene::initGameplay() builds the map, its colliders, item pickups (if applicable), camera, finish line, and player entities.

### SceneManager
Handles transitions between scenes and supports deferred scene switching like the assignments. This enables level transitions and switching between the main menu and gameplay.

### World
The world acts as a central coordinator, and stores entities, owns the systems, and runs the update loop. This ensures a consistent data -> logic -> render flow.

### Renderer
Built on top of the SDL3 renderer, with a lightweight abstraction layer. Renders sprites and levels, with the textures loaded via TextureManager.

### Asset Management
Loading of textures is centralized from the assignment guides, which allows for textures etc to be cached and reloaded later if needed.

### Event and GameState Handling
The global GameState tracks:
 - The player's health
 - Individual vehicle part health
 - Player lap counts

This is combined with event handling to switch between scenes when a player achieves a specified lap count.

# Gameplay-Specific Changes
Built on top of the original engine from the assignments, I have added:
 - Per-part damage to vehicles, instead of just overall health
 - Drifting movement with gradual rotation
 - Up to 2 players (WASD + arrow keys)

# Key Strengths
### Modular
Systems operate independently and are easy to modify when needed. This makes it easy to add new features without modifying the engine code.

### Extensible
New gameplay features are as simple as new components and systems, new maps via Tiled, and integrating with existing systems.

### Reusable
Engine is mostly separate from game logic, allowing the player to create their own Game.cpp and Scene.cpp based on a template to create the game.

# Current Limitations
Relies on a lot of manual component checks, which are error-prone.
No centralised physics handling.

# Summary
This engine is a simple 2D only ECS-based framework that provides the core gameplay systems, entity and scene management, rendering and animation, and a reusable architecture.
It is well suited for small 2D games, particularly vehicle-based ones.