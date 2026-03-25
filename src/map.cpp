#include "map.h"
#include "manager/TextureManager.h"
#include "vendor/tinyxml2.h"
#include <sstream>
#include <cmath>

void Map::load(const char *path, SDL_Texture *ts) {
    tileset = ts;
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path) != tinyxml2::XML_SUCCESS) {
        return;
    }

    //parse width and height of map
    auto* mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");

    //parse terrain data
    auto* layer = mapNode->FirstChildElement("layer");
    auto* data = layer->FirstChildElement("data");

    std::string csv = data->GetText();
    std::stringstream ss(csv);
    tileData = std::vector(height, std::vector<int>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::string val;
            //read chars from stringstream into val until it hits a comma, or until eof
            if (!std::getline(ss, val, ',')) break;
            tileData[i][j] = std::stoi(val);
        }
    }

    //parse collider data
    for (auto* objectGroup = layer->NextSiblingElement("objectgroup");
        objectGroup != nullptr;
        objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
            if (std::string(objectGroup->Name()) == std::string("Item Layer")) {
                for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                    Collider c;
                    c.rect.x = obj->FloatAttribute("x");
                    c.rect.y = obj->FloatAttribute("y");
                    c.rect.w = obj->FloatAttribute("width");
                    c.rect.h = obj->FloatAttribute("height");
                    items.push_back(c);
                }
                objectGroup = objectGroup->NextSiblingElement("objectgroup");
            }
            if (std::string(objectGroup->Name()) == std::string("Collision Layer")) {
                for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                    Collider c;
                    c.rect.x = obj->FloatAttribute("x");
                    c.rect.y = obj->FloatAttribute("y");
                    c.rect.w = obj->FloatAttribute("width");
                    c.rect.h = obj->FloatAttribute("height");
                    colliders.push_back(c);
                }
            }
        }
}

void Map::draw(const Camera &cam) {
    if (!tileset) {
        return;
    }
    
    SDL_FRect src{}, dst{};

    dst.w = dst.h = 32;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = tileData[row][col];

            float worldX = static_cast<float>(col) * dst.w;
            float worldY = static_cast<float>(row) * dst.h;

            dst.x = std::round(worldX - cam.view.x);
            dst.y = std::round(worldY - cam.view.y);

            src.w = 32;
            src.h = 32;
            switch (type) {
                case 1:
                    //dirt
                    src.x = 0;
                    src.y = 0;
                    break;
                case 2:
                    //grass
                    src.x = 32;
                    src.y = 0;
                    break;
                case 3:
                    //red
                    src.x = 0;
                    src.y = 32;
                    break;
                case 4:
                    //water
                    src.x = 32;
                    src.y = 32;
                    break;
                default:
                    break;
            }
            TextureManager::draw(tileset, src, dst);
        }
    }
}