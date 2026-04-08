#include "../Game.h"
#include "TextureManager.h"
#include "../ecs/Component.h"
#include "../main.h"

#include <iostream>
#include <ostream>

// doesn't use extra memory, just gets reference of the object
extern Game* game;
std::unordered_map<std::string, SDL_Texture*> TextureManager::textures;

// load texture with a path
SDL_Texture* TextureManager::load(const char* path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        Main::errors.emplace_back("Texture from ");
        Main::errors.emplace_back(path);
        Main::errors.emplace_back(" found in the cache.\n");
        return it->second;
    }

    // try to get the image at path provided
    SDL_Surface* temp_surface = IMG_Load(path);

    // if null try loading backup image
    if (!temp_surface) {
        Main::errors.emplace_back("Image not found at path provided, loading fallback image...\n");
        temp_surface = IMG_Load("./assets/img/ball.png");
        // if still null return null
        if (!temp_surface) {
            Main::errors.emplace_back("Fallback image retrieval failed.\n");
            return nullptr;
        }
    }

    Main::errors.emplace_back("Image found on disk.\n");

    // try creating texture from image
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, temp_surface);

    // free memory from loaded image
    SDL_DestroySurface(temp_surface);

    Main::errors.emplace_back("SDL texture created from image.\n");

    // if texture is invalid just return null
    if (!texture) return nullptr;

    textures[path] = texture;

    Main::errors.emplace_back("Created texture is not null.\n");

    return texture;
}

void TextureManager::draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst) {
    // draw texture to screen
    SDL_RenderTexture(game->renderer, texture, &src, &dst);
}

void TextureManager::draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst, float rotation, SDL_FPoint centre) {
    // draw texture to screen but rotated
    SDL_RenderTextureRotated(game->renderer, texture, &src, &dst, rotation, &centre, SDL_FLIP_NONE);
}

// delete loaded textures and free memory
void TextureManager::clean() {
    for (auto& tex : textures) {
        SDL_DestroyTexture(tex.second);
        tex.second = nullptr;
    }
    textures.clear();
}