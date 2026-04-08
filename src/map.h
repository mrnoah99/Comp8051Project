#include <vector>
#include <SDL3/SDL.h>
#include "ecs/Component.h"

class Map {
    public:
        Map() = default;
        ~Map() = default;

        void load(const char *path, SDL_Texture *track, SDL_Texture *grass);
        void draw(const Camera &cam);

        SDL_Texture *trackTileset = nullptr;
        SDL_Texture *grassTileset = nullptr;
        // initializing with {} makes all values inside set to 0 by default
        int width{}, height{};
        std::vector<std::vector<int>> grassTileData;
        std::vector<std::vector<int>> trackTileData;
        std::vector<Collider> colliders;
        std::vector<Collider> items;
        std::vector<Collider> finishLine;
        std::vector<Transform> startPositions;
        SDL_FRect cameraLocation{0,0, 1280,720};
    private:
        int grassGIDFirst = 0;
        int roadGIDFirst = 0;
};