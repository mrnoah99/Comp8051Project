#pragma once

#include "../Entity.h"
#include "../Component.h"

#include <vector>
#include <memory>

class World;

class CleanupSystem {
    public:
        void update(std::vector<std::unique_ptr<Entity>>& entities, World& world);
    private:
        int mapW;
        int mapH;
};