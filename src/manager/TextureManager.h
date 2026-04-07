#pragma once

#include <unordered_map>
#include <string>

#include <SDL3_image/SDL_image.h>

class TextureManager {
    public:
        static SDL_Texture* load(const char* path);

        // floating point rect = FRect, needs x, y, width, and height
        static void draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst);
        static void draw(SDL_Texture* texture, SDL_FRect src, SDL_FRect dst, float rotation, Sprite sprite);

        static void clean();
    private:
        static std::unordered_map<std::string, SDL_Texture*> textures;
};