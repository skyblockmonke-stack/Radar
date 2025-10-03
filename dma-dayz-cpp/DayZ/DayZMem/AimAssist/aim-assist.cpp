#include "aim-assist.h"
#include <iostream>

namespace DayZ {

AimAssist::AimAssist() : enabled(false) {
    // kmboxHandle = kmbox_init(); // Uncomment when kmbox SDK is available
}

void AimAssist::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool AimAssist::isEnabled() const {
    return enabled;
}

void AimAssist::update(std::shared_ptr<DayZ::Camera> camera, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, std::shared_ptr<DayZ::Scoreboard> scoreboard) {
    if (!enabled || !camera) return;

    // Find closest enemy within 100 units
    auto target = findClosestEnemy(camera, entities, scoreboard, 100.0f);
    if (!target) return;

    // Calculate target rotation
    float targetRotation = calculateTargetRotation(camera, target);

    // Activate aim assist
    activateAimAssist(targetRotation);

    /*
    // Uncomment when kmbox SDK is available and integrated
    if (enabled && camera) {
        // Find closest enemy within 100 units
        auto target = findClosestEnemy(camera, entities, scoreboard, 100.0f);
        if (target) {
            // Calculate target rotation
            float targetRotation = calculateTargetRotation(camera, target);

            // Calculate mouse deltas (assuming kmbox API)
            int deltaX = (targetRotation - camera->currentRotation) * sensitivity; // sensitivity to be defined
            int deltaY = 0; // For horizontal aim assist

            // Send to kmbox
            kmbox_mouse_move(kmboxHandle, deltaX, deltaY);
        }
    }
    */
}

std::shared_ptr<DayZ::Entity> AimAssist::findClosestEnemy(std::shared_ptr<DayZ::Camera> camera, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, std::shared_ptr<DayZ::Scoreboard> scoreboard, float maxDistance) {
    std::shared_ptr<DayZ::Entity> closestEnemy = nullptr;
    float closestDist = maxDistance;

    for (const auto& ent : entities) {
        if (!ent || !ent->isValid() || ent->isDead || !ent->isPlayer() || !ent->FutureVisualStatePtr) continue;

        // Exclude local player
        auto ident = ent->getPlayerIdentity(scoreboard.get());
        if (ident && ident->PlayerName) {
            std::string playerNameLower = ident->PlayerName->value;
            std::string mainPlayerNameLower = ""; // TODO: get local player name from config or context
            std::transform(playerNameLower.begin(), playerNameLower.end(), playerNameLower.begin(), ::tolower);
            std::transform(mainPlayerNameLower.begin(), mainPlayerNameLower.end(), mainPlayerNameLower.begin(), ::tolower);
            if (playerNameLower == mainPlayerNameLower) {
                continue;
            }
        }

        float dist = camera->InvertedViewTranslation.Dist(ent->FutureVisualStatePtr->position);
        if (dist < closestDist) {
            closestDist = dist;
            closestEnemy = ent;
        }
    }
    return closestEnemy;
}

float AimAssist::calculateTargetRotation(std::shared_ptr<DayZ::Camera> camera, std::shared_ptr<DayZ::Entity> target) {
    DMARender::Vector3 dirVec = target->FutureVisualStatePtr->position - camera->InvertedViewTranslation;
    float rotation = atan2(dirVec.y, dirVec.x) * (180.0f / 3.14159265f);
    return rotation;
}

void AimAssist::activateAimAssist(float targetRotation) {
    // Stub for aim assist activation
    std::cout << "AimAssist activated: target rotation = " << targetRotation << std::endl;
    // Integration with external API or input simulation to be implemented here

    /*
    // Uncomment when kmbox SDK is available
    // Calculate mouse deltas based on target rotation
    float currentRotation = 0.0f; // Get from camera or input
    int deltaX = (targetRotation - currentRotation) * 10; // sensitivity factor
    int deltaY = 0; // Horizontal only for now

    // Send mouse move command to kmbox
    kmbox_mouse_move(kmboxHandle, deltaX, deltaY);
    */
}

} // namespace DayZ
