#include "../Game.h"
#include "TextureManager.h"

#include <iostream>
#include <ostream>

// doesn't use extra memory, just gets reference of the object
extern Game* game;
std::unordered_map<std::string, SDL_Texture*> TextureManager::textures;

// load texture with a path
SDL_Texture* TextureManager::load(const char* path) {

    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }

    SDL_Surface* temp_surface = IMG_Load(path);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(game->renderer, temp_surface);

    SDL_DestroySurface(temp_surface);

    if (!texture) {
        return nullptr;
    }

    textures[path] = texture;

    return texture;
}

void TextureManager::draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst) {
    // draw texture to screen
    SDL_RenderTexture(game->renderer, texture, &src, &dst);
}

void TextureManager::clean() {
    for (auto& tex : textures) {
        SDL_DestroyTexture(tex.second);
        tex.second = nullptr;
    }
    textures.clear();
}