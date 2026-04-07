#pragma once

#include <memory>
#include <vector>

#include "../Entity.h"

class DestructionSystem {
    // vehicle damage system
    public:
        void update(std::vector<std::unique_ptr<Entity>>& entities) {
            for (auto& e : entities) {
                if (e->hasComponent<Velocity>() && e->hasComponent<Transform>()) {
                    // get required components
                    auto& t = e->getComponent<Transform>();
                    auto& v = e->getComponent<Velocity>();

                    // handle engine damage/repair
                    if (e->hasComponent<Engine>()) {
                        switch(e->getComponent<Engine>().partHealth.health) {
                            case 0:
                                // engine destroyed, move speed basically nonexistent
                                v.maxSpeed = 0.1f;
                                break;
                            case 1:
                                // engine very low health, move speed drastically reduced
                                v.maxSpeed = 10.0f;
                                break;
                            case 2:
                                // engine damaged, move speed reduced
                                v.maxSpeed = 45.0f;
                                break;
                            case 3:
                                // engine in good health, max speed allowed
                                v.maxSpeed = 100.0f;
                                break;
                            default:
                                // if somehow out of this range, speed is 0
                                v.maxSpeed = 0.0f;
                                break;
                        }
                    }
                    
                    // handle fuel tank/transmission damage/repair
                    if (e->hasComponent<FuelTransmission>()) {
                        switch(e->getComponent<FuelTransmission>().partHealth.health) {
                            case 0:
                                // fuel tank ruptured and transmission ruined, acceleration basically nonexistent
                                v.acceleration = 0.05f;
                                break;
                            case 1:
                                // fuel tank leaking and transmission very damaged, acceleration drastically lowered
                                v.acceleration = 0.1f;
                            case 2:
                                // fuel tank holding and transmission damaged, acceleration reduced
                                v.acceleration = 0.6f;
                                break;
                            case 3:
                                // fuel tank and transmission health good, full acceleration rate
                                v.acceleration = 1.0f;
                                break;
                            default:
                                // if somehow breaking the range, acceleration is 0
                                v.acceleration = 0.0f;
                                break;
                        }

                        // handle wheel damage/repair
                        if (e->hasComponent<LeftWheels>() && e->hasComponent<RightWheels>()) {
                            auto leftWheel = e->getComponent<LeftWheels>();
                            auto rightWheel = e->getComponent<RightWheels>();
                            switch(leftWheel.partHealth.health + rightWheel.partHealth.health) {
                                case 0:
                                    // wheels destroyed, rotation speed basically gone
                                    t.rotationSpeed = 0.05f;
                                    break;
                                case 1:
                                    // one side completely destroyed, other barely hanging in there
                                    t.rotationSpeed = 0.1f;
                                    break;
                                case 2:
                                    // heavy damage to one or both sides
                                    t.rotationSpeed = 0.25f;
                                    break;
                                case 3:
                                    // half hp between them
                                    t.rotationSpeed = 0.4f;
                                    break;
                                case 4:
                                    // small damage to one or both
                                    t.rotationSpeed = 0.55f;
                                    break;
                                case 5:
                                    // one hp damage to one side
                                    t.rotationSpeed = 0.8f;
                                    break;
                                case 6:
                                    // full health on both sides
                                    t.rotationSpeed = 1.0f;
                                    break;
                                default:
                                    // if somehow out of range, no turn for you >:)
                                    t.rotationSpeed = 0.0f;
                                    break;
                            }
                        }
                    }
                }
            }
        }
};