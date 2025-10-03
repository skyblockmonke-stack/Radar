#pragma once
#include "RadarAdapter.h"
#include "../../Structs/Scoreboard.h"
#include <unordered_set>

extern ImVec2 g_mainPlayerScreenPos;
extern ImVec2 g_mainPlayerScreenPosBuffered;

// Constructor
// #include "../../AimAssist/aim-assist.h"

DayZ::RadarAdapter::RadarAdapter(std::shared_ptr<DayZ::MemoryUpdater> memUpdater, std::shared_ptr<DMARender::RenderBridge> renderBridge)
	: memUpdater(memUpdater), renderBridge(renderBridge) {
	loadFavoriteSteamIDs("steamids.txt"); // Read "steamids.txt" for your favorite admins
	DayZ::Entity::initializeRareItems("rareitems.txt"); // Read "rareitems.txt" for rare itemlist, use TypeName e.g. CarBattery
	aimAssist = std::make_unique<DayZ::AimAssist>();
	aimAssist->setEnabled(false); // Disabled by default
}

// draw loot and dead entities with filters
#include <algorithm> // for std::transform
#include <cctype> // for std::tolower
#include <unordered_set>

void DayZ::RadarAdapter::drawLoot(DayZ::Camera* camera, DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities)
{
	if (!renderBridge) return; // If the render bridge is not initialized, return

	bool showVehicles = renderBridge->shouldShowVehicles();
	bool showBoats = renderBridge->shouldShowBoats();
	bool showGrounditems = renderBridge->shouldShowGrounditems();
	bool showDeadAnimals = renderBridge->shouldShowDeadAnimals();
	bool showDeadPlayers = renderBridge->shouldShowDeadPlayers();
	bool showWeapons = renderBridge->shouldShowWeapons();
	bool showClothing = renderBridge->shouldShowClothing();
	bool showBackpacks = renderBridge->shouldShowBackpacks();
	bool showProxyMagazines = renderBridge->shouldShowProxyMagazines();
	bool showFood = renderBridge->shouldShowFood();
	bool showAmmo = renderBridge->shouldShowAmmo();
	bool showRare = renderBridge->shouldShowRare();
	bool showOptics = renderBridge->shouldShowOptics();
	bool showBase = renderBridge->shouldShowBase();
	bool showMelee = renderBridge->shouldShowMelee();
	bool showExplosives = renderBridge->shouldShowExplosives();
	int radarFont2 = renderBridge->shouldRadarFont2();
	int blipSize2 = renderBridge->shouldBlipSize2();
	int lootDistanceDeadzone = renderBridge->shouldLootDistanceDeadzone();

	std::unordered_set<uint32_t> drawnNetworkIDs;

	auto mainPlayerName = renderBridge->shouldPlayerName();
	auto mainPlayerPos = g_mainPlayerScreenPos;

	for (auto const& item : entities) {
		if (!item->isValid())
			continue;

		// Exclude main player entity by name (case-insensitive)
		auto ident = item->getPlayerIdentity(memUpdater->getScoreboard().get());
		bool isMainPlayer = false;
		if (ident && ident->PlayerName) {
			std::string playerNameLower = ident->PlayerName->value;
			std::string mainPlayerNameLower = mainPlayerName;
			std::transform(playerNameLower.begin(), playerNameLower.end(), playerNameLower.begin(), ::tolower);
			std::transform(mainPlayerNameLower.begin(), mainPlayerNameLower.end(), mainPlayerNameLower.begin(), ::tolower);
			if (playerNameLower == mainPlayerNameLower) {
				isMainPlayer = true;
			}
		}
		if (isMainPlayer)
			continue;

		// Avoid duplicate blips by NetworkID
		if (drawnNetworkIDs.find(item->NetworkID) != drawnNetworkIDs.end())
			continue;
		drawnNetworkIDs.insert(item->NetworkID);

		std::string postFix = "";

		ImU32 textCol;

		float dist = camera->InvertedViewTranslation.Dist(item->FutureVisualStatePtr->position);

		// Exclude entities too close to main player position (within 1.5 units)
		auto screenPos = WorldToRadar(curMap, mTransform, item->FutureVisualStatePtr->position);
		float dx = screenPos.x - mainPlayerPos.x;
		float dy = screenPos.y - mainPlayerPos.y;
		float distToMainPlayer = sqrt(dx * dx + dy * dy);
		if (distToMainPlayer < 1.5f)
			continue;

		// Color for Items and Dead players/animals
		if (item->isPlayer() && item->isDead && showDeadPlayers) {
			textCol = IM_COL32(0, 255, 255, 255);
			postFix = " (Dead)";
		} else if (item->isAnimal() && item->isDead && showDeadAnimals) {
			textCol = IM_COL32(0, 255, 0, 255);
			postFix = " (Dead)";
		} else if (item->isCar() && showVehicles) {
			textCol = IM_COL32(255, 0, 245, 255);
		} else if (item->isBoat() && showBoats) {
			textCol = IM_COL32(255, 0, 245, 255);
		} else if (item->isRare() && showRare) {
			textCol = IM_COL32(255, 0, 255, 255);
		} else if (item->isBackpack() && showBackpacks) {
			textCol = IM_COL32(175, 255, 0, 255);
		} else if (item->isClothing() && showClothing) {
			textCol = IM_COL32(255, 255, 0, 255);
		} else if (item->isWeapon() && showWeapons) {
			textCol = IM_COL32(255, 0, 100, 255);
		} else if (item->isProxyMagazines() && showProxyMagazines) {
			textCol = IM_COL32(255, 117, 50, 255);
		} else if (item->isFood() && showFood) {
			textCol = IM_COL32(50, 140, 50, 255);
		} else if (item->isAmmo() && showAmmo) {
			textCol = IM_COL32(255, 117, 50, 255);
		} else if (item->isGroundItem() && showGrounditems) {
			textCol = IM_COL32(255, 255, 255, 255);
		} else if (item->isOptic() && showOptics) {
			textCol = IM_COL32(0, 150, 255, 255);
		} else if (item->isBase() && showBase) {
			textCol = IM_COL32(0, 150, 255, 255);
		} else if (item->isMelee() && showMelee) {
			textCol = IM_COL32(0, 150, 255, 255);
		} else if (item->isExplosives() && showExplosives) {
			textCol = IM_COL32(255, 0, 120, 255);
		} else {
			continue;
		}

		if (dist < lootDistanceDeadzone)
			continue;

		auto bestName = item->EntityTypePtr->getBestString();
		std::string displayName = bestName ? bestName->value : "Unknown";

		// Filter out debug or internal names
		if (displayName.find("dz\\characters") != std::string::npos || displayName.find("bodyparts") != std::string::npos) {
			displayName = "Unknown";
		}

		drawBlip(screenPos, blipSize2, textCol, radarFont2, 1, { displayName + postFix });

	}
}

void DayZ::RadarAdapter::drawAliveEntities(DayZ::Camera* camera, DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, Scoreboard* scoreboard)
{
	// neutralTransform for my followplayer function
	DMARender::MapTransform neutralTransform = mTransform;
	neutralTransform.dragOffsetX = 0;
	neutralTransform.dragOffsetY = 0;
	static ImVec2 lastValidScreenPos = ImVec2(0, 0);

	if (!renderBridge) return; // If the render bridge is not initialized, return

	bool showZombies = renderBridge->shouldShowZombies();
	bool showAnimals = renderBridge->shouldShowAnimals();
	std::string mainPlayerName = renderBridge->shouldPlayerName();
	int blipSize = renderBridge->shouldBlipSize();
	int aimLineLength = renderBridge->shouldAimlinelength();
	int radarFont = renderBridge->shouldRadarFont();
	int zombiesBlipSize = renderBridge->shouldZombiesBlipSize();
	int animalsBlipSize = renderBridge->shouldAnimalsBlipSize();

	std::unordered_set<uint32_t> drawnNetworkIDs;

	for (const auto& ent : entities) {
		if (!ent->isValid() || ent->isDead)
			continue;

		ImU32 blipColor;
		bool isMainPlayer = false;
		std::vector<std::string> infoText;

		// neutral MainPlayerPos
		ImVec2 neutralPos = WorldToRadar(curMap, neutralTransform, ent->FutureVisualStatePtr->position);
		// final "MainPlayerScreenPos"
		ImVec2 screenPos = WorldToRadar(curMap, mTransform, ent->FutureVisualStatePtr->position);

		if (ent->isPlayer()) {
			auto ident = ent->getPlayerIdentity(scoreboard);
			if (ident && ident->PlayerName) {
				std::string playerNameLower = ident->PlayerName->value;
				std::string mainPlayerNameLower = mainPlayerName;
				std::transform(playerNameLower.begin(), playerNameLower.end(), playerNameLower.begin(), ::tolower);
				std::transform(mainPlayerNameLower.begin(), mainPlayerNameLower.end(), mainPlayerNameLower.begin(), ::tolower);
				if (playerNameLower == mainPlayerNameLower) {
					isMainPlayer = true;
				}
			}
		}

		// Exclude main player from being drawn twice
		if (isMainPlayer) {
			// update global (static) Pos if MainPlayer
			float smoothFactor = 0.5f;  // Adjustable
			g_mainPlayerScreenPos = ImVec2(
				lastValidScreenPos.x + (neutralPos.x - lastValidScreenPos.x) * smoothFactor,
				lastValidScreenPos.y + (neutralPos.y - lastValidScreenPos.y) * smoothFactor
			);
			lastValidScreenPos = g_mainPlayerScreenPos;

			// Skip drawing this blip here to avoid duplicate
			continue;
		}

		// Avoid duplicate blips by NetworkID
		if (drawnNetworkIDs.find(ent->NetworkID) != drawnNetworkIDs.end())
			continue;
		drawnNetworkIDs.insert(ent->NetworkID);

		// Exclude entities too close to main player position (within 1.5 units)
		float dx = screenPos.x - g_mainPlayerScreenPos.x;
		float dy = screenPos.y - g_mainPlayerScreenPos.y;
		float distToMainPlayer = sqrt(dx * dx + dy * dy);
		if (distToMainPlayer < 1.5f)
			continue;

		// ALIFE ENTITIES colors
		if (ent->isPlayer()) {
			blipColor = IM_COL32(255, 0, 0, 255);
		}
		else if (ent->isZombie() && showZombies) {
			blipColor = IM_COL32(255, 255, 0, 255);
		}
		else if (ent->isAnimal() && showAnimals) {
			blipColor = IM_COL32(0, 255, 0, 130);
		}
		else {
			continue;
		}

		float dist = camera->InvertedViewTranslation.Dist(ent->FutureVisualStatePtr->position);

		if (ent->isAnimal()) {
			auto entBestStr = ent->EntityTypePtr->getBestString();
			if (entBestStr) {
				auto entName = std::string(entBestStr->value);
				infoText.push_back(entName);
			}
		}
		if (ent->isPlayer()) {
			auto ident = ent->getPlayerIdentity(scoreboard);
			if (ident && ident->PlayerName && !isMainPlayer) {
				infoText.push_back(ident->PlayerName->value);
			}

			if (ent->InventoryPtr->isHandItemValid && !isMainPlayer) {
				auto bestHandStr = ent->InventoryPtr->handItem->EntityTypePtr->getBestString();
				if (bestHandStr) {
					infoText.push_back(bestHandStr->value);
				}
				else {
					infoText.push_back("(Unknown)");
				}
			}
			if (!isMainPlayer) {
				infoText.push_back(std::format("{:.0f}m", dist));
			}
		}

		if (ent->isPlayer()) {
			drawBlipDirectional(screenPos, blipSize, blipColor, radarFont, 1, infoText, ent->FutureVisualStatePtr->getRotationCorrected(), aimLineLength);
		}
		else if (ent->isZombie()) {
			drawBlip(screenPos, zombiesBlipSize, blipColor, radarFont, 1, infoText);
		}
		else if (ent->isAnimal()) {
			drawBlip(screenPos, animalsBlipSize, blipColor, radarFont, 1, infoText);
		}
		else {
			continue;
		}
	}
}

void DayZ::RadarAdapter::drawPlayerList(DayZ::Camera* camera, Scoreboard* scoreboard) {
	static bool includeSlowEntities = false; // Checkbox-Status

	// load small table font
	//ImGui::PushFont(tableFont);
	ImGui::Begin("Local Player List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// add checkbox for SlowEntityTable, experimenal to find invis admins
	ImGui::Checkbox("Include SlowEntityTable", &includeSlowEntities);

	// slow entity table
	std::set<uint32_t> slowEntityIDs;
	if (includeSlowEntities) {
		for (const auto& slowEntity : memUpdater->getSlowEntityTable()->resolvedEntities) {
			if (slowEntity->isPlayer() && slowEntity->isValid()) {
				slowEntityIDs.insert(slowEntity->NetworkID);
			}
		}
	}

	// Imgui table setup
	if (ImGui::BeginTable("PlayerTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Distance");
		ImGui::TableSetupColumn("Player Name");
		//ImGui::TableSetupColumn("Steam ID");
		//ImGui::TableSetupColumn("Network ID");
		//ImGui::TableSetupColumn("Model Name");
		ImGui::TableHeadersRow();

		// different approach to iterate over entities tables
		auto processEntities = [&](const std::vector<std::shared_ptr<DayZ::Entity>>& entities) {
			for (auto& entity : entities) {
				if (!entity->isPlayer() || !entity->isValid()) continue;

				auto ident = entity->getPlayerIdentity(scoreboard);
				if (!ident) continue;

				// bad working validation
				const char* playerName = (ident->PlayerName && ident->PlayerName->value)
					? ident->PlayerName->value
					: "Unknown";
				const char* steamID = (ident->SteamID && ident->SteamID->value)
					? ident->SteamID->value
					: "Unknown";

				uint32_t networkID = entity->NetworkID;

				// Add Modelname
				//const char* modelName = (entity->EntityTypePtr && entity->EntityTypePtr->ModelName && entity->EntityTypePtr->ModelName->value)
				//	? entity->EntityTypePtr->ModelName->value
				//	: "Unknown";

				float distance = camera->InvertedViewTranslation.Dist(entity->FutureVisualStatePtr->position);

				// dont show self
				//if (distance < 3.1)
					//continue;

				// different color for SlowEntityTable
				bool isSlowEntity = slowEntityIDs.count(networkID) > 0;
				ImU32 rowColor = isSlowEntity ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 255, 255);

				ImGui::TableNextRow(ImGuiTableRowFlags_None, 0);
				ImGui::PushStyleColor(ImGuiCol_Text, rowColor);

				// Distance
				ImGui::TableNextColumn();
				ImGui::Text("%dm", static_cast<int>(distance)); // Ohne Nachkommastelle

				// Player Name
				ImGui::TableNextColumn();
				ImGui::Text("%s", playerName);

				// Network ID
				//ImGui::TableNextColumn();
				//ImGui::Text("%u", networkID);

				// Model Name
				//ImGui::TableNextColumn();
				//ImGui::Text("%s", modelName);

				//ImGui::PopStyleColor();
			}
			};

		
		processEntities(memUpdater->getNearEntityTable()->resolvedEntities);
		processEntities(memUpdater->getFarEntityTable()->resolvedEntities);

		ImGui::EndTable();
	}
	//ImGui::PopFont(); // back to default font
	ImGui::End();
}

void DayZ::RadarAdapter::drawServerPlayerList(std::shared_ptr<DayZ::Scoreboard> scoreboard) {

	std::unordered_set<int> seenNetworkIDs;

		// table setup
	//ImGui::PushFont(tableFont);
	ImGui::Begin("Server Player List", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Click to Copy Profile Link to Clipboard");
	if (ImGui::BeginTable("ServerPlayerTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("#"); 
		ImGui::TableSetupColumn("Player Name");
		ImGui::TableSetupColumn("Steam ID");
		/*ImGui::TableSetupColumn("Network ID");*/
		ImGui::TableHeadersRow();

		int rowIndex = 1; // start from 1

		for (const auto& identity : scoreboard->resolvedIdentities) {
			if (!identity) continue;

			// bad working validation
			if (identity->NetworkID <= 9999 || 
				!identity->SteamID || !identity->SteamID->value ||
				strncmp(identity->SteamID->value, "7656", 4) != 0 ||
				!identity->PlayerName || !identity->PlayerName->value || !seenNetworkIDs.insert(identity->NetworkID).second) {
				continue; 
			}

			const char* playerName = identity->PlayerName->value;
			const char* steamID = identity->SteamID->value;
			uint32_t networkID = identity->NetworkID;

			// Check for favorite Steam IDs
			bool isFavorite = favoriteSteamIDs.count(steamID) > 0;
			ImU32 rowColor = isFavorite ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 255, 255, 255); // green for "admins"

			ImGui::TableNextRow();
			ImGui::PushStyleColor(ImGuiCol_Text, rowColor);

			// numbering
			ImGui::TableNextColumn();
			ImGui::Text("%d", rowIndex++);

			// Player Name
			ImGui::TableNextColumn();
			ImGui::Text("%s", playerName);

			// Steam ID with clickable functionality
			ImGui::TableNextColumn();
			if (ImGui::Selectable(steamID, false, ImGuiSelectableFlags_SpanAllColumns)) {
				std::string steamLink = "https://steamcommunity.com/profiles/";
				steamLink += steamID;
				ImGui::SetClipboardText(steamLink.c_str()); // copy to clipboard
			}
			ImGui::PopStyleColor();
			// Network ID
			//ImGui::TableNextColumn();
			//ImGui::Text("%u", networkID);
		}

		//ImGui::PopFont();
		ImGui::EndTable();
	}

	ImGui::End();
}

void DayZ::RadarAdapter::DrawOverlay(DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform)
{
	auto camera = memUpdater->getCamera();
	auto scoreboardPtr = memUpdater->getScoreboard();

	if (!camera || !scoreboardPtr) {
		ImGui::Text("DMA not attached or DayZ not running. Attach to process first.");
		return; // Prevent crashes if camera or scoreboard are null
	}

	if (!curMap) {
		ImGui::Text("Map not loaded. Ensure you are in a supported map (Chernarus, Livonia, etc.).");
		return; // Prevent crashes if map is not loaded
	}

	//ImGui::PushFont(radarFont);
	//ImGui::PopFont();

	// draw radar entities/items
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getSlowEntityTable()->resolvedEntities);
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getItemTable()->resolvedEntities);
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getNearEntityTable()->resolvedEntities);
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getFarEntityTable()->resolvedEntities);
	drawAliveEntities(camera.get(), curMap, mTransform, memUpdater->getFarEntityTable()->resolvedEntities, scoreboardPtr.get());
	drawAliveEntities(camera.get(), curMap, mTransform, memUpdater->getNearEntityTable()->resolvedEntities, scoreboardPtr.get());

	// ImGui toggle for aim assist - commented out to prevent map loading issues
	// TODO: re-enable after resolving ImGui context issues
	/*
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Aim Assist Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		bool enabled = aimAssist ? aimAssist->isEnabled() : false;
		if (ImGui::Checkbox("Enable Aim Assist", &enabled)) {
			if (aimAssist) {
				aimAssist->setEnabled(enabled);
			}
		}
		ImGui::End();
	}
	*/

	// Update aim assist with near and far entities - commented out to fix map loading
	/*
	if (aimAssist) {
		aimAssist->update(camera, memUpdater->getNearEntityTable()->resolvedEntities, scoreboardPtr);
		aimAssist->update(camera, memUpdater->getFarEntityTable()->resolvedEntities, scoreboardPtr);
	}
	*/

	//ImGui::PopFont();

	// ImGui toggle for aim assist - commented out to prevent issues
	/*
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Aim Assist Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		bool enabled = aimAssist ? aimAssist->isEnabled() : false;
		if (ImGui::Checkbox("Enable Aim Assist", &enabled)) {
			if (aimAssist) {
				aimAssist->setEnabled(enabled);
			}
		}
		ImGui::End();
	}
	*/

	// draw localplayerlist
	if (renderBridge->shouldShowPlayerList()) {
		drawPlayerList(camera.get(), scoreboardPtr.get());
	}
	// draw globalplayerlist from scoreboard
	if (renderBridge->shouldShowServerPlayerList()) {
		drawServerPlayerList(scoreboardPtr);
	}

	// Aim assist update (commented out by default to keep application functional)
	// if (aimAssist && aimAssist->isEnabled()) {
	// 	aimAssist->update(camera, memUpdater->getNearEntityTable()->resolvedEntities, scoreboardPtr);
	// 	aimAssist->update(camera, memUpdater->getFarEntityTable()->resolvedEntities, scoreboardPtr);
	// }
}

void DayZ::RadarAdapter::createFonts()
{
		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 1;
		config.GlyphExtraSpacing.x = 1.0f;
		radarFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 12, &config);

		// add small font
		ImFontConfig configSmall;
		configSmall.SizePixels = 12.0f; // reduce size?
		tableFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14, &configSmall);
}

void DayZ::RadarAdapter::loadFavoriteSteamIDs(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Error: SteamID file could not be opened: " << filePath << std::endl;
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty()) {
			favoriteSteamIDs.insert(line); // Add "Admin"SteamIDs
		}
	}

	file.close();
}
