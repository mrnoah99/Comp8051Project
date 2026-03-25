#include "Scene.h"
#include "../manager/AssetManager.h"
#include "../Game.h"

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

        auto& settingsOverlay = createSettingsOverlay(windowWidth, windowHeight);
        createCogButton(windowWidth, windowHeight, settingsOverlay);
}

void Scene::initGameplay(const char* mapPath, int windowWidth, int windowHeight) {
    //spawn wall entities
    world.getMap().load(mapPath, TextureManager::load("C:/Users/noah/Documents/Gamedev 2/Comp8051/Assignment10 - not for marks/assets/tileset.png"));
    for (auto &collider : world.getMap().colliders) {
        auto& e = world.createEntity();
        e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f, 1.0f);
        auto& c = e.addComponent<Collider>("wall");
        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;

        //add visual to colliders
        SDL_Texture* tex = TextureManager::load("C:/Users/noah/Documents/Gamedev 2/Comp8051/Assignment10 - not for marks/assets/tileset.png");
        SDL_FRect colSrc{0,32,32,32};
        SDL_FRect colDst{c.rect.x, c.rect.y, c.rect.w, c.rect.h};
        e.addComponent<Sprite>(tex, colSrc, colDst);
    }

    for (auto &collider : world.getMap().items) {
        auto& item(world.createEntity());
        auto& c = item.addComponent<Collider>("item");

        c.rect.x = collider.rect.x;
        c.rect.y = collider.rect.y;
        c.rect.w = collider.rect.w;
        c.rect.h = collider.rect.h;

        auto& t = item.addComponent<Transform>(Vector2D(c.rect.x, c.rect.y), 0.0f, 1.0f);
        
        SDL_Texture* itemTex = TextureManager::load("C:/Users/noah/Documents/Gamedev 2/Comp8051/Assignment10 - not for marks/assets/coin.png");
        SDL_FRect itemSrc{0, 0, 32, 32};
        SDL_FRect itemDst{t.position.x, t.position.y, 32, 32};
        item.addComponent<Sprite>(itemTex, itemSrc, itemDst);
    }

    //add camera
    auto& camera = world.createEntity();
    SDL_FRect camView{};
    camView.w = windowWidth;
    camView.h = windowHeight;
    camera.addComponent<Camera>(camView, world.getMap().width * 32.0f, world.getMap().height * 32.0f);

    //add player
    auto& player(world.createEntity());
    auto& playerTransform = player.addComponent<Transform>(Vector2D(0,0), 0.0f, 1.0f);
    player.addComponent<Velocity>(Vector2D(0.0f,0.0f), 100.0f);
    player.addComponent<PlayerTag>();
    player.addComponent<Health>(Game::gameState.playerHealth, Game::gameState.playerHealth);

    Animation anim = AssetManager::getAnimation("player");
    player.addComponent<Animation>(anim);
    
    SDL_Texture* texture = TextureManager::load("C:/Users/noah/Documents/Gamedev 2/Comp8051/Assignment10 - not for marks/assets/animations/fox_anim.png");
    SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];
    SDL_FRect playerDst {playerTransform.position.x, playerTransform.position.y, 64, 64};
    player.addComponent<Sprite>(texture, playerSrc, playerDst);

    auto& playerCollider = player.addComponent<Collider>("player");
    playerCollider.rect.w = playerDst.w; playerCollider.rect.h = playerDst.h;

    auto& spawner(world.createEntity());
    Transform t = spawner.addComponent<Transform>(Vector2D(windowWidth/2, windowHeight - 5), 0.0f, 1.0f);
    spawner.addComponent<TimedSpawner>(2.0f, [this, t] {
        auto& entity(world.createDeferredEntity());
        entity.addComponent<Transform>(Vector2D(t.position.x, t.position.y), 0.0f, 1.0f);
        entity.addComponent<Velocity>(Vector2D(0, -1), 100.0f);

        Animation anim = AssetManager::getAnimation("enemy");
        entity.addComponent<Animation>(anim);

        SDL_Texture* tex = TextureManager::load("C:/Users/noah/Documents/Gamedev 2/Comp8051/Assignment10 - not for marks/assets/animations/bird_anim.png");
        SDL_FRect src {0, 0, 32, 32};
        SDL_FRect dst {t.position.x, t.position.y, 32, 32};
        entity.addComponent<Sprite>(tex, src, dst);

        auto& collider = entity.addComponent<Collider>("projectile");
        collider.rect.x = dst.w;
        collider.rect.y = dst.h;

        entity.addComponent<ProjectileTag>();
    });

    auto &state(world.createEntity());
    state.addComponent<SceneState>();

    world.setMovementBoundary();
}

Entity& Scene::createSettingsOverlay(int windowWidth, int windowHeight) {
    auto &overlay(world.createEntity());
    SDL_Texture *overlayTex = TextureManager::load("../assets/ui/settings.jng");
    SDL_FRect overlaySrc {0, 0, windowWidth * 0.85f, windowHeight * 0.85f};
    SDL_FRect overlayDst {(float)windowWidth / 2 - overlaySrc.w / 2, (float) windowHeight / 2 - overlaySrc.h / 2, overlaySrc.w, overlaySrc.h};
    overlay.addComponent<Transform>(Vector2D(overlayDst.x, overlayDst.y), 0.0f, 1.0f);
    overlay.addComponent<Sprite>(overlayTex, overlaySrc, overlayDst, RenderLayer::UI, false);
    createSettingsUIComponents(overlay);
    return overlay;
}

Entity& Scene::createCogButton(int windowWidth, int windowHeight, Entity& overlay){
    auto &cog(world.createEntity());
    auto& cogTransform = cog.addComponent<Transform>(Vector2D((float) windowWidth - 50, (float) windowHeight - 50), 0.0f, 1.0f);

    SDL_Texture *tex = TextureManager::load("../assets/ui/cog.png");
    SDL_FRect cogSrc {0, 0, 32, 32};
    SDL_FRect cogDst {cogTransform.position.x, cogTransform.position.y, cogSrc.w, cogSrc.h};
    cog.addComponent<Sprite>(tex, cogSrc, cogDst, RenderLayer::UI);
    cog.addComponent<Collider>("ui", cogDst);
    auto& clickable = cog.addComponent<Clickable>();
    clickable.onPressed = [&cogTransform] {
        cogTransform.scale = 0.5f;
    };
    clickable.onReleased = [this, &overlay, &cogTransform] {
        cogTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay);
    };
    clickable.onCancel = [&cogTransform] {
        cogTransform.scale = 1.0f;
    };
    return cog;
}

void Scene::createSettingsUIComponents(Entity& overlay) {
    if (!overlay.hasComponent<Children>()) {
        overlay.addComponent<Children>();
    }

    auto& overlayTransform = overlay.getComponent<Transform>();
    auto& overlaySprite = overlay.getComponent<Sprite>();

    float baseX = overlayTransform.position.x;
    float baseY = overlayTransform.position.y;

    auto& closeButton = world.createEntity();
    auto& closeTransform = closeButton.addComponent<Transform>(Vector2D(baseX + overlaySprite.dst.x - 40, baseY + 10), 0.0f, 1.0f);

    SDL_Texture *tex = TextureManager::load("../assets/ui/close.png");
    SDL_FRect closeSrc {0, 0, 32, 32};
    SDL_FRect closeDst {closeTransform.position.x, closeTransform.position.y, closeSrc.w, closeSrc.h};
    closeButton.addComponent<Sprite>(tex, closeSrc, closeDst, RenderLayer::UI, false);
    closeButton.addComponent<Collider>("ui", closeDst);

    auto& clickable = closeButton.addComponent<Clickable>();
    clickable.onPressed = [&closeTransform] {
        closeTransform.scale = 0.5f;
    };
    clickable.onReleased = [this, &overlay, &closeTransform] {
        closeTransform.scale = 1.0f;
        toggleSettingsOverlayVisibility(overlay);
    };
    clickable.onCancel = [&closeTransform] {
        closeTransform.scale = 1.0f;
    };

    closeButton.addComponent<Parent>(&overlay);
    auto& parentChildren = overlay.getComponent<Children>();
    parentChildren.children.push_back(&closeButton);
}

void Scene::toggleSettingsOverlayVisibility(Entity& overlay) {
    auto& sprite = overlay.getComponent<Sprite>();

    bool newVisibility = !sprite.visible;
    sprite.visible = newVisibility;
    if (overlay.hasComponent<Children>()) {
        auto& c = overlay.getComponent<Children>();

        for (auto& child : c.children) {
            if (child && child->hasComponent<Sprite>()) {
                child->getComponent<Sprite>().visible = newVisibility;
            }

            if (child && child->hasComponent<Collider>()) {
                child->getComponent<Collider>().enabled = newVisibility;
            }
        }
    }
}