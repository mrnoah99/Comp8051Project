#include "map.h"
#include "manager/TextureManager.h"
#include "vendor/tinyxml2.h"
#include "main.h"

#include <sstream>
#include <cmath>

void Map::load(const char *path, SDL_Texture *track, SDL_Texture *grass) {
    trackTileset = track;
    grassTileset = grass;
    
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path) != tinyxml2::XML_SUCCESS) {
        return;
    }
    Main::errors.emplace_back("Map loaded...\n");

    //parse width and height of map
    auto* mapNode = doc.FirstChildElement("map");
    width = mapNode->IntAttribute("width");
    height = mapNode->IntAttribute("height");

    auto* tilesetData = mapNode->FirstChildElement("tileset");
    if (tilesetData->IntAttribute("firstgid") == 1) tilesetData = tilesetData->NextSiblingElement("tileset");
    if (tilesetData->IntAttribute("firstgid") == 15) {
        grassGIDFirst = 0;
        roadGIDFirst = 14;
    } else if (tilesetData->IntAttribute("firstgid") == 91) {
        grassGIDFirst = 90;
        roadGIDFirst = 1000;
    }

    //parse terrain data
    auto* layer = mapNode->FirstChildElement("layer");
    
    trackTileData = std::vector(height, std::vector<int>(width));
    grassTileData = std::vector(height, std::vector<int>(width));

    Main::errors.emplace_back("Map data read and tile data processing initiated...\n");

    if (layer) {
        auto* data = layer->FirstChildElement("data");
        std::string csv = data->GetText();
        std::stringstream ss(csv);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                std::string val;
                //read chars from stringstream into val until it hits a comma, or until eof
                if (!std::getline(ss, val, ',')) break;

                int gid = std::stoi(val);
                if (gid >= grassGIDFirst && gid < roadGIDFirst) gid = gid - grassGIDFirst;
                else if (gid > roadGIDFirst) gid = gid - roadGIDFirst;

                if (std::string(layer->Attribute("name")) == std::string("Track")) {
                    trackTileData[i][j] = gid;
                } else if (std::string(layer->Attribute("name")) == std::string("Grass")) {
                    grassTileData[i][j] = gid;
                }
            }
        }
    }
    auto* layer2 = layer->NextSiblingElement("layer");

    if (layer2) {
        auto* data = layer2->FirstChildElement("data");
        std::string csv = data->GetText();
        std::stringstream ss(csv);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                std::string val;
                //read chars from stringstream into val until it hits a comma, or until eof
                if (!std::getline(ss, val, ',')) break;

                int gid = std::stoi(val);
                if (gid >= grassGIDFirst && gid < roadGIDFirst) gid = gid - grassGIDFirst;
                else if (gid > roadGIDFirst) gid = gid - roadGIDFirst;

                if (std::string(layer2->Attribute("name")) == std::string("Track")) {
                    trackTileData[i][j] = gid;
                } else if (std::string(layer2->Attribute("name")) == std::string("Grass")) {
                    grassTileData[i][j] = gid;
                }
            }
        }
    }

    Main::errors.emplace_back("Terrain layers data read, reading colliders...\n");

    //parse collider data
    for (auto* objectGroup = mapNode->FirstChildElement("objectgroup");
        objectGroup != nullptr;
        objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
            std::string objectGroupName = std::string(objectGroup->Attribute("name"));
            if (objectGroupName == std::string("Item Layer")) {
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
            if (objectGroupName == std::string("Walls")) {
                for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                    Collider c;
                    c.rect.x = obj->FloatAttribute("x");
                    c.rect.y = obj->FloatAttribute("y");
                    c.rect.w = obj->FloatAttribute("width");
                    c.rect.h = obj->FloatAttribute("height");
                    c.rotation = obj->FloatAttribute("rotation")*(SDL_PI_F/180.0f);
                    colliders.push_back(c);
                }
            }

            if (objectGroupName == std::string("FinishLine")) {
                for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                    Collider c;
                    c.rect.x = obj->FloatAttribute("x");
                    c.rect.y = obj->FloatAttribute("y");
                    c.rect.w = obj->FloatAttribute("width");
                    c.rect.h = obj->FloatAttribute("height");
                    c.rotation = obj->FloatAttribute("rotation")*(SDL_PI_F/180.0f);
                    finishLine.push_back(c);
                }
                Main::errors.emplace_back("Finish line colliders added...\n");
            }

            if (objectGroupName == std::string("StartPositions")) {
                for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {
                    Transform t;
                    t.position.x = obj->FloatAttribute("x");
                    t.position.y = obj->FloatAttribute("y");
                    t.rotation = obj->FloatAttribute("rotation");
                    startPositions.emplace_back(t);
                    Main::errors.emplace_back("Start position added...\n");
                }
            }

            if (objectGroupName == std::string("Camera")) {
                auto* obj = objectGroup->FirstChildElement("object");
                if (obj) {
                    cameraLocation.x = obj->FloatAttribute("x");
                    cameraLocation.y = obj->FloatAttribute("y");
                    cameraLocation.w = obj->FloatAttribute("width");
                    cameraLocation.h = obj->FloatAttribute("height");
                    Main::errors.emplace_back("Camera location found...\n");
                }
            }
        }

        Main::errors.emplace_back("Object data loaded. Map initialising...\n");
}

void Map::draw(const Camera &cam) {
    if (!trackTileset || !grassTileset) {
        return;
    }
    
    SDL_FRect src{}, dst{};

    dst.w = dst.h = 128;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = grassTileData[row][col];

            if (type == 0) continue;

            float worldX = static_cast<float>(col) * dst.w;
            float worldY = static_cast<float>(row) * dst.h;

            dst.x = std::round(worldX - cam.view.x);
            dst.y = std::round(worldY - cam.view.y);

            src.w = 128;
            src.h = 128;
            switch (type) {
                case 1:
                    // dirt bottom right
                    src.x = 0;
                    src.y = 0;
                    break;
                case 2:
                    // dirt bottom left
                    src.x = 128;
                    src.y = 0;
                    break;
                case 3:
                    // dirt left
                    src.x = 256;
                    src.y = 0;
                    break;
                case 4:
                    // grass
                    src.x = 384;
                    src.y = 0;
                    break;
                case 5:
                    // dirt right
                    src.x = 512;
                    src.y = 0;
                    break;
                case 6:
                    // dirt top right
                    src.x = 640;
                    src.y = 0;
                    break;
                case 7:
                    // dirt top left
                    src.x = 768;
                    src.y = 0;
                    break;
                case 8:
                    // grass top right
                    src.x = 0;
                    src.y = 128;
                    break;
                case 9:
                    // grass top
                    src.x = 128;
                    src.y = 128;
                    break;
                case 10:
                    // grass top left
                    src.x = 256;
                    src.y = 128;
                    break;
                case 11:
                    // dirt
                    src.x = 384;
                    src.y = 128;
                    break;
                case 12:
                    // grass bottom right
                    src.x = 512;
                    src.y = 128;
                    break;
                case 13:
                    // grass bottom
                    src.x = 640;
                    src.y = 128;
                    break;
                case 14:
                    // grass bottom left
                    src.x = 768;
                    src.y = 128;
                    break;
                default:
                    break;
            }
            TextureManager::draw(grassTileset, src, dst);
        }
    }

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = trackTileData[row][col];

            if (type == 0) continue;

            float worldX = static_cast<float>(col) * dst.w;
            float worldY = static_cast<float>(row) * dst.h;

            dst.x = std::round(worldX - cam.view.x);
            dst.y = std::round(worldY - cam.view.y);

            src.w = 128;
            src.h = 128;
            switch (type) {
                case 1:
                    // straight up/down thin
                    src.x = 0;
                    src.y = 0;
                    break;
                case 2:
                    // straight left/right thin
                    src.x = 128;
                    src.y = 0;
                    break;
                case 3:
                    // rotate up->right thin
                    src.x = 256;
                    src.y = 0;
                    break;
                case 4:
                    // rotate right->down thin
                    src.x = 384;
                    src.y = 0;
                    break;
                case 5:
                    // rotate left->up thin
                    src.x = 512;
                    src.y = 0;
                    break;
                case 6:
                    // rotate down->left thin
                    src.x = 640;
                    src.y = 0;
                    break;
                case 7:
                    // start up thin
                    src.x = 768;
                    src.y = 0;
                    break;
                case 8:
                    // start right thin
                    src.x = 896;
                    src.y = 0;
                    break;
                case 9:
                    // start down thin
                    src.x = 1024;
                    src.y = 0;
                    break;
                case 10:
                    // start left thin
                    src.x = 1152;
                    src.y = 0;
                    break;
                case 11:
                    // finish line up/down thin
                    src.x = 0;
                    src.y = 128;
                    break;
                case 12:
                    // finish line left/right thin
                    src.x = 128;
                    src.y = 128;
                    break;
                case 13:
                    // junction up
                    src.x = 256;
                    src.y = 128;
                    break;
                case 14:
                    // junction right
                    src.x = 384;
                    src.y = 128;
                    break;
                case 15:
                    // junction down
                    src.x = 512;
                    src.y = 128;
                    break;
                case 16:
                    // junction left
                    src.x = 640;
                    src.y = 128;
                    break;
                case 17:
                    // dead end up
                    src.x = 768;
                    src.y = 128;
                    break;
                case 18:
                    // dead end right
                    src.x = 896;
                    src.y = 128;
                    break;
                case 19:
                    // dead end down
                    src.x = 1024;
                    src.y = 128;
                    break;
                case 20:
                    // dead end left
                    src.x = 1152;
                    src.y = 128;
                    break;
                case 21:
                    // open up
                    src.x = 0;
                    src.y = 256;
                    break;
                case 22:
                    // open right
                    src.x = 128;
                    src.y = 256;
                    break;
                case 23:
                    // open down
                    src.x = 256;
                    src.y = 256;
                    break;
                case 24:
                    // open left
                    src.x = 384;
                    src.y = 256;
                    break;
                case 25:
                    // left wall
                    src.x = 512;
                    src.y = 256;
                    break;
                case 26:
                    // top wall
                    src.x = 640;
                    src.y = 256;
                    break;
                case 27:
                    // right wall
                    src.x = 768;
                    src.y = 256;
                    break;
                case 28:
                    // bottom wall
                    src.x = 896;
                    src.y = 256;
                    break;
                case 29:
                    // empty space
                    src.x = 1024;
                    src.y = 256;
                    break;
                case 30:
                    // sharp curve right
                    src.x = 1152;
                    src.y = 256;
                    break;
                case 31:
                    // sharp curve down
                    src.x = 0;
                    src.y = 384;
                    break;
                case 32:
                    // sharp curve up
                    src.x = 128;
                    src.y = 384;
                    break;
                case 33:
                    // sharp curve left
                    src.x = 256;
                    src.y = 384;
                    break;
                case 34:
                    // top corners
                    src.x = 384;
                    src.y = 384;
                    break;
                case 35:
                    // right corners
                    src.x = 512;
                    src.y = 384;
                    break;
                case 36:
                    // bottom corners
                    src.x = 640;
                    src.y = 384;
                    break;
                case 37:
                    // left corners
                    src.x = 768;
                    src.y = 384;
                    break;
                case 38:
                    // all corners
                    src.x = 896;
                    src.y = 384;
                    break;
                case 39:
                    // top right
                    src.x = 1024;
                    src.y = 384;
                    break;
                case 40:
                    // bottom right
                    src.x = 1152;
                    src.y = 384;
                    break;
                case 41:
                    // bottom left
                    src.x = 0;
                    src.y = 512;
                    break;
                case 42:
                    // top left
                    src.x = 128;
                    src.y = 512;
                    break;
                case 43:
                    // curve up 1
                    src.x = 256;
                    src.y = 512;
                    break;
                case 44:
                    // curve up 2
                    src.x = 384;
                    src.y = 512;
                    break;
                case 45:
                    // curve right 1
                    src.x = 512;
                    src.y = 512;
                    break;
                case 46:
                    // curve right 2
                    src.x = 640;
                    src.y = 512;
                    break;
                case 47:
                    // curve down 1
                    src.x = 768;
                    src.y = 512;
                    break;
                case 48:
                    // curve down 2
                    src.x = 896;
                    src.y = 512;
                    break;
                case 49:
                    // curve left 1
                    src.x = 1024;
                    src.y = 512;
                    break;
                case 50:
                    // curve left 2
                    src.x = 1152;
                    src.y = 512;
                    break;
                case 51:
                    // curve up 3
                    src.x = 0;
                    src.y = 640;
                    break;
                case 52:
                    // curve right 3
                    src.x = 128;
                    src.y = 640;
                    break;
                case 53:
                    // curve down 3
                    src.x = 256;
                    src.y = 640;
                    break;
                case 54:
                    // curve left 3
                    src.x = 384;
                    src.y = 640;
                    break;
                case 55:
                    // diagonal upright steep L1
                    src.x = 512;
                    src.y = 640;
                    break;
                case 56:
                    // diagonal upright steep R1
                    src.x = 640;
                    src.y = 640;
                    break;
                case 57:
                    // diagonal upleft steep L1
                    src.x = 768;
                    src.y = 640;
                    break;
                case 58:
                    // diagonal upleft steep R1
                    src.x = 896;
                    src.y = 640;
                    break;
                case 59:
                    // diagonal upleft L1
                    src.x = 1024;
                    src.y = 640;
                    break;
                case 60:
                    // diagonal upleft R1
                    src.x = 1152;
                    src.y = 640;
                    break;
                case 61:
                    // diagonal upright L1
                    src.x = 0;
                    src.y = 768;
                    break;
                case 62:
                    // diagonal upright R1
                    src.x = 128;
                    src.y = 768;
                    break;
                case 63:
                    // diagonal upright steep L2
                    src.x = 256;
                    src.y = 768;
                    break;
                case 64:
                    // diagonal upright steep R2
                    src.x = 384;
                    src.y = 768;
                    break;
                case 65:
                    // diagonal upleft steep L2
                    src.x = 512;
                    src.y = 768;
                    break;
                case 66:
                    // diagonal upleft steep R2
                    src.x = 640;
                    src.y = 768;
                    break;
                case 67:
                    // diagonal upright L2
                    src.x = 768;
                    src.y = 768;
                    break;
                case 68:
                    // diagonal upright R2
                    src.x = 896;
                    src.y = 768;
                    break;
                case 69:
                    // diagonal upleft L2
                    src.x = 1024;
                    src.y = 768;
                    break;
                case 70:
                    // diagonal upleft R2
                    src.x = 1152;
                    src.y = 768;
                    break;
                case 71:
                    // diagonal upright steep L3
                    src.x = 0;
                    src.y = 896;
                    break;
                case 72:
                    // diagonal upright steep R3
                    src.x = 128;
                    src.y = 896;
                    break;
                case 73:
                    // diagonal upleft L3
                    src.x = 256;
                    src.y = 896;
                    break;
                case 74:
                    // diagonal upleft R3
                    src.x = 384;
                    src.y = 896;
                    break;
                case 75:
                    // diagonal upleft steep L3
                    src.x = 512;
                    src.y = 896;
                    break;
                case 76:
                    // diagonal upleft steep R3
                    src.x = 640;
                    src.y = 896;
                    break;
                case 77:
                    // diagonal upright L3
                    src.x = 768;
                    src.y = 896;
                    break;
                case 78:
                    // diagonal upright R3
                    src.x = 896;
                    src.y = 896;
                    break;
                case 79:
                    // start up
                    src.x = 1024;
                    src.y = 896;
                    break;
                case 80:
                    // start right
                    src.x = 1152;
                    src.y = 896;
                    break;
                case 81:
                    // start down
                    src.x = 0;
                    src.y = 1024;
                    break;
                case 82:
                    // start left
                    src.x = 128;
                    src.y = 1024;
                    break;
                case 83:
                    // finish line left
                    src.x = 256;
                    src.y = 1024;
                    break;
                case 84:
                    // finish line middle
                    src.x = 384;
                    src.y = 1024;
                    break;
                case 85:
                    // finish line right
                    src.x = 512;
                    src.y = 1024;
                    break;
                case 86:
                    // finish line top
                    src.x = 640;
                    src.y = 1024;
                    break;
                case 87:
                    // finish line middle 2
                    src.x = 768;
                    src.y = 1024;
                    break;
                case 88:
                    // finish line bottom
                    src.x = 896;
                    src.y = 1024;
                    break;
                case 89:
                    // pit
                    src.x = 1024;
                    src.y = 1024;
                    break;
                case 90:
                    // triangle thing
                    src.x = 1152;
                    src.y = 1024;
                    break;
                default:
                    break;
            }
            TextureManager::draw(trackTileset, src, dst);
        }
    }
}