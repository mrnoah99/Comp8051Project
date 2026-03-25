#include <vector>
#include <SDL3/SDL.h>
#include "ecs/Component.h"

class Map {
    public:
        Map() = default;
        ~Map() = default;

        void load(const char *path, SDL_Texture *ts);
        void draw(const Camera &cam);

        SDL_Texture *tileset = nullptr;
        // initializing with {} makes all values inside set to 0 by default
        int width{}, height{};
        std::vector<std::vector<int>> tileData;
        std::vector<Collider> colliders;
        std::vector<Collider> items;
};