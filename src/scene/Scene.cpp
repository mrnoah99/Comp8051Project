#include "Scene.h"
#include "../manager/AssetManager.h"
#include "../Game.h"
#include "../main.h"

Scene::Scene(SceneType type, const std::string& sceneName, const std::string& mapPath, const int windowWidth, const int windowHeight) : name(sceneName), type(type) {
    
    if (type == SceneType::MainMenu) {
        initMainMenu(windowWidth, windowHeight);
        return;
    }

    initGameplay(mapPath.c_str(), windowWidth, windowHeight);
}

void Scene::initMainMenu(int windowWidth, int windowHeight) {
    auto &cam = world.createEntity();
        cam.addComponent<Camera>();

        auto &menu(world.createEntity());
        menu.addComponent<Transform>(Vector2D(0,0), 0.0f, 1.0f);
        SDL_Texture *tex = TextureManager::load("C:/Users/noah/Documents/Gamedev 2/Comp8051/Assignment10 - not for marks/assets/menu.png");
        SDL_FRect menuSrc{0, 0, (float) windowWidth, (float) windowHeight};
        SDL_FRect menuDst{0, 0, menuSrc.w, menuSrc.h};
        menu.addComponent<Sprite>(tex, menuSrc, menuDst);

        // auto& settingsOverlay = createSettingsOverlay(windowWidth, windowHeight);
        // createCogButton(windowWidth, windowHeight, settingsOverlay);
}

void Scene::initGameplay(const char* mapPath, int windowWidth, int windowHeight) {
    SDL_Texture* track = TextureManager::load("./assets/maps/road_tileset.png");
    SDL_Texture* grass = TextureManager::load("./assets/maps/grass_tileset.png");

    bool p2 = world.getMap().startPositions.size() > 1;
    world.player2 = p2;

    world.getMap().load(mapPath, track, grass);
    for (auto &collider : world.getMap().colliders) {
        auto& e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, collider.rotation*(SDL_PI_F/180.0f));
        auto& c = e.addComponent<Collider>("wall");
        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;
        
        c.tag = "Wall";
        c.halfSize = Vector2D(c.rect.w/2, c.rect.h/2) * 0.8;
        c.centre = Vector2D(c.rect.x, c.rect.y) + Vector2D(c.rect.w/2, c.rect.h/2);
        c.rotation = collider.rotation;
    }

    Main::errors.emplace_back("Colliders created...\n");

    for (auto &collider : world.getMap().items) {
        auto& item(world.createEntity());
        auto& c = item.addComponent<Collider>("item");

        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;

        auto& t = item.addComponent<Transform>(Vector2D(c.rect.x, c.rect.y), 0.0f, 1.0f);
        
        SDL_Texture* itemTex = TextureManager::load("./assets/maps/coin.png");
        SDL_FRect itemSrc{0, 0, 32, 32};
        SDL_FRect itemDst{t.position.x, t.position.y, 32, 32};
        item.addComponent<Sprite>(itemTex, itemSrc, itemDst);
    }

    Main::errors.emplace_back("Items created...\n");

    //add camera
    auto& camera = world.createEntity();
    SDL_FRect camView = world.getMap().cameraLocation;
    if (camView.w < windowWidth) camView.w = windowWidth;
    if (camView.h < windowHeight) camView.h = windowHeight;
    camera.addComponent<Camera>(camView, world.getMap().width * 128.0f, world.getMap().height * 128.0f);
    
    auto& cameraBackup = world.createEntity();
    cameraBackup.addComponent<CameraLocation>(world.getMap().cameraLocation);

    Main::errors.emplace_back("Camera created...\n");

    // add player
    // get spawn position if applicable
    Vector2D spawnLoc;
    float spawnRotation;
    if (world.getMap().startPositions.empty()) {
        Main::errors.emplace_back("Start positions empty, placing player at default location...\n");
        spawnLoc = Vector2D(20.0f, 30.0f);
        spawnRotation = 0.0f;
    }
    else {
        Main::errors.emplace_back("Spawn location found, positioning player...\n");
        spawnLoc = world.getMap().startPositions.front().position;
        spawnRotation = world.getMap().startPositions.front().rotation;
    }

    // create player entity w/ position and velocity
    auto& player = world.createEntity();
    auto& playerTransform = player.addComponent<Transform>(spawnLoc, spawnRotation, 1.0f, 1.0f);
    player.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f, 150.0f, 3.0f);

    // add sprite
    SDL_Texture* texture = TextureManager::load("./assets/animations/Cars/car_red_small_2.png");
    SDL_FRect playerSrc{0, 0, 40, 62};
    SDL_FRect playerDst {0, 0, 40, 62};
    if (texture) player.addComponent<Sprite>(texture, playerSrc, playerDst);
    else Main::errors.emplace_back("player texture failed to load\n");
    
    // player tag and health
    player.addComponent<PlayerTag>();
    player.addComponent<Health>(Game::gameState.player1Health, Game::gameState.player1Health);

    VehiclePartHealth vehicleComponent;
    vehicleComponent.health = vehicleComponent.maxHealth = 3;
    vehicleComponent.updateHealth = [](VehiclePartHealth& part, int n) {
        part.health += n;
        if (part.health > part.maxHealth)
            part.health = part.maxHealth;
        if (part.health < 0) part.health = 0;
    };

    player.addComponent<Engine>(vehicleComponent);
    player.addComponent<FuelTransmission>(vehicleComponent);
    player.addComponent<LeftWheels>(vehicleComponent);
    player.addComponent<RightWheels>(vehicleComponent);

    Main::errors.emplace_back("Player 1 components applied...\n");

    // collider
    auto& playerCollider = player.addComponent<Collider>();
    playerCollider.tag = "player";
    playerCollider.halfSize = Vector2D(playerDst.w / 2, playerDst.h / 2) * 0.8;
    playerCollider.centre = playerTransform.position + Vector2D(playerDst.w / 2, playerDst.h / 2);
    playerCollider.rotation = playerTransform.rotation * (SDL_PI_F/180.0f);
    playerCollider.right = Vector2D(std::cos(playerCollider.rotation), std::sin(playerCollider.rotation));
    playerCollider.forward = Vector2D(-std::sin(playerCollider.rotation), std::cos(playerCollider.rotation));

    Main::errors.emplace_back("Player 1 spawned...\n");

    if (p2) {
        // get player 2 spawn
        spawnLoc = world.getMap().startPositions.back().position;
        spawnRotation = world.getMap().startPositions.back().rotation;

        // create player entity w/ position and velocity
        auto& player2 = world.createEntity();
        auto& player2Transform = player2.addComponent<Transform>(spawnLoc, spawnRotation, 1.0f, 1.0f);
        player2.addComponent<Velocity>(Vector2D(0.0f, 0.0f), 0.0f, 150.0f, 3.0f);

        // add sprite
        SDL_Texture* texture2 = TextureManager::load("./assets/animations/Cars/car_blue_small_2.png");
        SDL_FRect player2Src{0, 0, 40, 62};
        SDL_FRect player2Dst {0, 0, 40, 62};
        if (texture2) player2.addComponent<Sprite>(texture2, player2Src, player2Dst);
        else Main::errors.emplace_back("player texture failed to load\n");
        
        // player tag and health
        player2.addComponent<Player2Tag>();
        player2.addComponent<Health>(Game::gameState.player2Health, Game::gameState.player2Health);

        VehiclePartHealth vehicle2Component;
        vehicle2Component.health = vehicle2Component.maxHealth = 3;
        vehicle2Component.updateHealth = [](VehiclePartHealth& part, int n) {
            part.health += n;
            if (part.health > part.maxHealth)
                part.health = part.maxHealth;
            if (part.health < 0) part.health = 0;
        };

        player2.addComponent<Engine>(vehicle2Component);
        player2.addComponent<FuelTransmission>(vehicle2Component);
        player2.addComponent<LeftWheels>(vehicle2Component);
        player2.addComponent<RightWheels>(vehicle2Component);

        Main::errors.emplace_back("Player 2 components applied...\n");

        // collider
        auto& player2Collider = player2.addComponent<Collider>();
        player2Collider.tag = "player";
        player2Collider.halfSize = Vector2D(player2Dst.w / 2, player2Dst.h / 2) * 0.8;
        player2Collider.centre = player2Transform.position + Vector2D(player2Dst.w / 2, player2Dst.h / 2);
        player2Collider.rotation = player2Transform.rotation * (SDL_PI_F/180.0f);
        player2Collider.right = Vector2D(std::cos(player2Collider.rotation), std::sin(player2Collider.rotation));
        player2Collider.forward = Vector2D(-std::sin(player2Collider.rotation), std::cos(player2Collider.rotation));

        Main::errors.emplace_back("Player 2 spawned...\n");
    }

    auto &state(world.createEntity());
    state.addComponent<SceneState>();

    world.setMovementBoundary();

    auto& finishPos = world.getMap().finishLine.front();
    auto& finishLine = world.createEntity();
    auto& finishTransform = finishLine.addComponent<Transform>(Vector2D(finishPos.rect.x, finishPos.rect.y), 0.0f, 0.0f, 1.0f);
    finishLine.addComponent<FinishLineTag>();
    auto& finishCollider = finishLine.addComponent<Collider>();
    finishCollider.centre = finishTransform.position + Vector2D(finishPos.rect.w / 2, finishPos.rect.h / 2);
    finishCollider.tag = "Finish";
    finishCollider.rotation = finishPos.rotation;
    finishCollider.right = Vector2D(std::cos(finishCollider.rotation), std::sin(finishCollider.rotation));
    finishCollider.forward = Vector2D(-std::sin(finishCollider.rotation), std::cos(finishCollider.rotation));

    Main::errors.emplace_back("Finish line collider spawned...\n");
}

// Entity& Scene::createSettingsOverlay(int windowWidth, int windowHeight) {
//     auto &overlay(world.createEntity());
//     SDL_Texture *overlayTex = TextureManager::load("../assets/ui/settings.jng");
//     SDL_FRect overlaySrc {0, 0, windowWidth * 0.85f, windowHeight * 0.85f};
//     SDL_FRect overlayDst {(float)windowWidth / 2 - overlaySrc.w / 2, (float) windowHeight / 2 - overlaySrc.h / 2, overlaySrc.w, overlaySrc.h};
//     overlay.addComponent<Transform>(Vector2D(overlayDst.x, overlayDst.y), 0.0f, 1.0f);
//     overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDst, RenderLayer::UI, false);
//     createSettingsUIComponents(overlay);
//     return overlay;
// }

// Entity& Scene::createCogButton(int windowWidth, int windowHeight, Entity& overlay){
//     auto &cog(world.createEntity());
//     auto& cogTransform = cog.addComponent<Transform>(Vector2D((float) windowWidth - 50, (float) windowHeight - 50), 0.0f, 1.0f);

//     SDL_Texture *tex = TextureManager::load("../assets/ui/cog.png");
//     SDL_FRect cogSrc {0, 0, 32, 32};
//     SDL_FRect cogDst {cogTransform.position.x, cogTransform.position.y, cogSrc.w, cogSrc.h};
//     cog.addComponent<Sprite>(tex, cogSrc, cogDst, RenderLayer::UI);
//     cog.addComponent<Collider>("ui", cogDst);
//     auto& clickable = cog.addComponent<Clickable>();
//     clickable.onPressed = [&cogTransform] {
//         cogTransform.scale = 0.5f;
//     };
//     clickable.onReleased = [this, &overlay, &cogTransform] {
//         cogTransform.scale = 1.0f;
//         toggleSettingsOverlayVisibility(overlay);
//     };
//     clickable.onCancel = [&cogTransform] {
//         cogTransform.scale = 1.0f;
//     };
//     return cog;
// }

// void Scene::createSettingsUIComponents(Entity& overlay) {
//     if (!overlay.hasComponent<Children>()) {
//         overlay.addComponent<Children>();
//     }

//     auto& overlayTransform = overlay.getComponent<Transform>();
//     auto& overlaySprite = overlay.getComponent<Sprite>();

//     float baseX = overlayTransform.position.x;
//     float baseY = overlayTransform.position.y;

//     auto& closeButton = world.createEntity();
//     auto& closeTransform = closeButton.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.x - 40, baseY + 10), 0.0f, 1.0f);

//     SDL_Texture *tex = TextureManager::load("../assets/ui/close.png");
//     SDL_FRect closeSrc {0, 0, 32, 32};
//     SDL_FRect closeDst {closeTransform.position.x, closeTransform.position.y, closeSrc.w, closeSrc.h};
//     closeButton.addComponent<Sprite>(tex, closeSrc, closeDst, RenderLayer::UI, false);
//     closeButton.addComponent<Collider>("ui", closeDst);

//     auto& clickable = closeButton.addComponent<Clickable>();
//     clickable.onPressed = [&closeTransform] {
//         closeTransform.scale = 0.5f;
//     };
//     clickable.onReleased = [this, &overlay, &closeTransform] {
//         closeTransform.scale = 1.0f;
//         toggleSettingsOverlayVisibility(overlay);
//     };
//     clickable.onCancel = [&closeTransform] {
//         closeTransform.scale = 1.0f;
//     };

//     closeButton.addComponent<Parent>(&overlay);
//     auto& parentChildren = overlay.getComponent<Children>();
//     parentChildren.children.push_back(&closeButton);
// }

// void Scene::toggleSettingsOverlayVisibility(Entity& overlay) {
//     auto& sprite = overlay.getComponent<Sprite>();

//     bool newVisibility = !sprite.visible;
//     sprite.visible = newVisibility;
//     if (overlay.hasComponent<Children>()) {
//         auto& c = overlay.getComponent<Children>();

//         for (auto& child : c.children) {
//             if (child && child->hasComponent<Sprite>()) {
//                 child->getComponent<Sprite>().visible = newVisibility;
//             }

//             if (child && child->hasComponent<Collider>()) {
//                 child->getComponent<Collider>().enabled = newVisibility;
//             }
//         }
//     }
// }