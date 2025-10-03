#pragma once

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>
#include "../../Structs/Entity.h"
#include "../../Structs/Scoreboard.h"
#include "../../Structs/Camera.h"
// #include "kmbox/kmbox.h" // Uncomment when kmbox SDK is available

namespace DayZ {

class AimAssist {
public:
    AimAssist();

    // Enable or disable aim assist
    void setEnabled(bool enabled);
    bool isEnabled() const;

    // Update aim assist logic with current camera, entities, and scoreboard
    void update(std::shared_ptr<DayZ::Camera> camera, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, std::shared_ptr<DayZ::Scoreboard> scoreboard);

private:
    bool enabled;
    // int kmboxHandle; // Uncomment when kmbox SDK is available

    // Find closest enemy player within maxDistance
    std::shared_ptr<DayZ::Entity> findClosestEnemy(std::shared_ptr<DayZ::Camera> camera, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, std::shared_ptr<DayZ::Scoreboard> scoreboard, float maxDistance);

    // Calculate rotation angle to target entity from camera position
    float calculateTargetRotation(std::shared_ptr<DayZ::Camera> camera, std::shared_ptr<DayZ::Entity> target);

    // Activate aim assist towards target rotation (stub for integration)
    void activateAimAssist(float targetRotation);
};

} // namespace DayZ
