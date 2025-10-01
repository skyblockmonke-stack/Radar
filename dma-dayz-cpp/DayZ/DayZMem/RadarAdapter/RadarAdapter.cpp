'DayZ::RadarAdapter::drawAliveEntities': function does not take 6 arguments
'void DayZ::RadarAdapter::drawAliveEntities(DayZ::Camera *,DMARender::IGameMap *,const DMARender::MapTransform &,const std::vector<std::shared_ptr<DayZ::Entity>,std::allocator<std::shared_ptr<DayZ::Entity>>> &,DayZ::Scoreboard *,std::unordered_set<uint32_t,std::hash<uint32_t>,std::equal_to<uint32_t>,std::allocator<uint32_t>> &)': overloaded member function not found in 'DayZ::RadarAdapter'
illegal reference to non-static member 'DayZ::RadarAdapter::renderBridge'
'DMARender::IRadar::WorldToRadar': a call of a non-static member function requires an object
'DMARender::IRadar::WorldToRadar': a call of a non-static member function requires an object
'DMARender::IRadar::drawBlipDirectional': a call of a non-static member function requires an object
'DMARender::IRadar::drawBlip': a call of a non-static member function requires an object
'DMARender::IRadar::drawBlip': a call of a non-static member function requires an object
'DayZ::RadarAdapter::drawAliveEntities': function does not take 6 arguments#pragma once
#include "RadarAdapter.h"
#include "../../Structs/Scoreboard.h"
#include <unordered_set>
#include <unordered_set>

extern ImVec2 g_mainPlayerScreenPos;
extern ImVec2 g_mainPlayerScreenPosBuffered;

// Constructor
DayZ::RadarAdapter::RadarAdapter(std::shared_ptr<DayZ::MemoryUpdater> memUpdater, std::shared_ptr<DMARender::RenderBridge> renderBridge)
	: memUpdater(memUpdater), renderBridge(renderBridge) {
	loadFavoriteSteamIDs("steamids.txt"); // Read "steamids.txt" for your favorite admins
	DayZ::Entity::initializeRareItems("rareitems.txt"); // Read "rareitems.txt" for rare itemlist, use TypeName e.g. CarBattery
	createFonts(); // Ensure fonts are created during initialization
}

// draw loot and dead entities with filters
#include <algorithm> // for std::transform
#include <cctype> // for std::tolower
#include <unordered_set>

void DayZ::RadarAdapter::drawLoot(DayZ::Camera* camera, DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, std::unordered_set<uint32_t>& drawnNetworkIDs)
{
	if (!renderBridge) return; // If the render bridge is not initialized, return

	// Temporarily disabled renderBridge flag checks to test rendering
	// bool showVehicles = renderBridge->shouldShowVehicles();
	// bool showBoats = renderBridge->shouldShowBoats();
	// bool showGrounditems = renderBridge->shouldShowGrounditems();
	// bool showDeadAnimals = renderBridge->shouldShowDeadAnimals();
	// bool showDeadPlayers = renderBridge->shouldShowDeadPlayers();
	// bool showWeapons = renderBridge->shouldShowWeapons();
	// bool showClothing = renderBridge->shouldShowClothing();
	// bool showBackpacks = renderBridge->shouldShowBackpacks();
	// bool showProxyMagazines = renderBridge->shouldShowProxyMagazines();
	// bool showFood = renderBridge->shouldShowFood();
	// bool showAmmo = renderBridge->shouldShowAmmo();
	// bool showRare = renderBridge->shouldShowRare();
	// bool showOptics = renderBridge->shouldShowOptics();
	// bool showBase = renderBridge->shouldShowBase();
	// bool showMelee = renderBridge->shouldShowMelee();
	// bool showExplosives = renderBridge->shouldShowExplosives();
	// int radarFont2 = renderBridge->shouldRadarFont2();
	// int blipSize2 = renderBridge->shouldBlipSize2();
	// int lootDistanceDeadzone = renderBridge->shouldLootDistanceDeadzone();

	bool showVehicles = true;
	bool showBoats = true;
	bool showGrounditems = true;
	bool showDeadAnimals = true;
	bool showDeadPlayers = true;
	bool showWeapons = true;
	bool showClothing = true;
	bool showBackpacks = true;
	bool showProxyMagazines = true;
	bool showFood = true;
	bool showAmmo = true;
	bool showRare = true;
	bool showOptics = true;
	bool showBase = true;
	bool showMelee = true;
	bool showExplosives = true;
	int radarFont2 = 12;
	int blipSize2 = 4;
	int lootDistanceDeadzone = 0;

	auto mainPlayerName = renderBridge->shouldPlayerName();
	auto mainPlayerPos = g_mainPlayerScreenPos;

	for (auto const& item : entities) {
		if (!item->isValid() || !item->FutureVisualStatePtr || !item->InventoryPtr || !item->EntityTypePtr)
			continue;

		// Re-enabled: Exclude main player entity by name (case-insensitive)
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

		// Re-enabled: Avoid duplicate blips by NetworkID
		if (drawnNetworkIDs.find(item->NetworkID) != drawnNetworkIDs.end())
			continue;
		drawnNetworkIDs.insert(item->NetworkID);

		std::string postFix = "";

		ImU32 textCol;

		float dist = camera->InvertedViewTranslation.Dist(item->FutureVisualStatePtr->position);

		// Re-enabled: Exclude entities too close to main player position (within 1.5 units)
		auto screenPos = this->WorldToRadar(curMap, mTransform, item->FutureVisualStatePtr->position);
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

		// Re-enabled: if (dist < lootDistanceDeadzone)
		if (dist < lootDistanceDeadzone)
			continue;

		auto bestName = item->EntityTypePtr->getBestString();
		std::string displayName = bestName ? bestName->value : "Unknown";

		// Filter out debug or internal names
		if (displayName.find("dz\\characters") != std::string::npos || displayName.find("bodyparts") != std::string::npos) {
			displayName = "Unknown";
		}

		std::string displayNameWithPostfix = displayName + postFix;
		drawBlip(screenPos, blipSize2, textCol, radarFont2, 1, { displayNameWithPostfix });

	}
}

static void DayZ::RadarAdapter::drawAliveEntities(DayZ::Camera* camera, DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, Scoreboard* scoreboard, std::unordered_set<uint32_t>& drawnNetworkIDs, std::shared_ptr<DMARender::RenderBridge> renderBridge)
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

	for (const auto& ent : entities) {
		totalEntities++;
		if (!ent->isValid() || ent->isDead) {
			skippedInvalid++;
			continue;
		}

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
			skippedMainPlayer++;
			// update global (static) Pos if MainPlayer
			// Removed smoothing to ensure immediate update
			g_mainPlayerScreenPos = neutralPos;
			lastValidScreenPos = g_mainPlayerScreenPos;

			// Skip drawing this blip here to avoid duplicate
			continue;
		}

		// Avoid duplicate blips by NetworkID
		if (drawnNetworkIDs.find(ent->NetworkID) != drawnNetworkIDs.end())
			continue;
		drawnNetworkIDs.insert(ent->NetworkID);

		// Temporarily disabled: Exclude entities too close to main player position (within 1.5 units)
		// float dx = screenPos.x - g_mainPlayerScreenPos.x;
		// float dy = screenPos.y - g_mainPlayerScreenPos.y;
		// float distToMainPlayer = sqrt(dx * dx + dy * dy);
		// if (distToMainPlayer < 1.5f)
		// 	continue;

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
				std::string distStr = std::format("{:.0f}m", dist);
				infoText.push_back(distStr);
			}
		}

		if (ent->isPlayer()) {
			this->drawBlipDirectional(screenPos, blipSize, blipColor, radarFont, 1, infoText, ent->FutureVisualStatePtr->getRotationCorrected(), aimLineLength);
			drawnBlips++;
		}
		else if (ent->isZombie()) {
			this->drawBlip(screenPos, zombiesBlipSize, blipColor, radarFont, 1, infoText);
			drawnBlips++;
		}
		else if (ent->isAnimal()) {
			this->drawBlip(screenPos, animalsBlipSize, blipColor, radarFont, 1, infoText);
			drawnBlips++;
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
	if (!curMap) return;

	auto camera = memUpdater->getCamera();
	auto scoreboardPtr = memUpdater->getScoreboard();

	if (!camera || !scoreboardPtr) {
		return; // Prevent crashes if camera or scoreboard are null
	}

	// Debug info
	static int drawnBlips = 0;
	static int totalEntities = 0;
	static int skippedMainPlayer = 0;
	static int skippedDuplicate = 0;
	static int skippedClose = 0;
	static int skippedInvalid = 0;
	drawnBlips = 0; // Reset each frame
	totalEntities = 0;
	skippedMainPlayer = 0;
	skippedDuplicate = 0;
	skippedClose = 0;
	skippedInvalid = 0;
	ImGui::Begin("Radar Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Camera: %s", camera ? "Valid" : "Null");
	ImGui::Text("Scoreboard: %s", scoreboardPtr ? "Valid" : "Null");
	ImGui::Text("SlowEntityTable: %zu", memUpdater->getSlowEntityTable()->resolvedEntities.size());
	ImGui::Text("ItemTable: %zu", memUpdater->getItemTable()->resolvedEntities.size());
	ImGui::Text("NearEntityTable: %zu", memUpdater->getNearEntityTable()->resolvedEntities.size());
	ImGui::Text("FarEntityTable: %zu", memUpdater->getFarEntityTable()->resolvedEntities.size());
	ImGui::Text("RadarFont: %s", radarFont ? "Valid" : "Null");
	ImGui::Text("Total Entities: %d", totalEntities);
	ImGui::Text("Skipped Invalid: %d", skippedInvalid);
	ImGui::Text("Skipped Main Player: %d", skippedMainPlayer);
	ImGui::Text("Skipped Duplicate: %d", skippedDuplicate);
	ImGui::Text("Skipped Close: %d", skippedClose);
	ImGui::Text("Drawn Blips: %d", drawnBlips);
	ImGui::End();

	std::unordered_set<uint32_t> drawnNetworkIDs;

	if (radarFont) ImGui::PushFont(radarFont);

	// draw radar entities/items
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getSlowEntityTable()->resolvedEntities, drawnNetworkIDs);
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getItemTable()->resolvedEntities, drawnNetworkIDs);
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getNearEntityTable()->resolvedEntities, drawnNetworkIDs);
	drawLoot(camera.get(), curMap, mTransform, memUpdater->getFarEntityTable()->resolvedEntities, drawnNetworkIDs);
	drawAliveEntities(camera.get(), curMap, mTransform, memUpdater->getFarEntityTable()->resolvedEntities, scoreboardPtr.get(), drawnNetworkIDs, renderBridge);
	drawAliveEntities(camera.get(), curMap, mTransform, memUpdater->getNearEntityTable()->resolvedEntities, scoreboardPtr.get(), drawnNetworkIDs, renderBridge);

	if (radarFont) ImGui::PopFont();

	// draw localplayerlist
	if (renderBridge && renderBridge->shouldShowPlayerList()) {
		drawPlayerList(camera.get(), scoreboardPtr.get());
	}
	// draw globalplayerlist from scoreboard
	if (renderBridge && renderBridge->shouldShowServerPlayerList()) {
		drawServerPlayerList(scoreboardPtr);
	}
}

void DayZ::RadarAdapter::createFonts()
{
	if (!ImGui::GetCurrentContext()) {
		std::cerr << "[ERROR] ImGui context not created in RadarAdapter::createFonts." << std::endl;
		return;
	}
	std::string fontPath = "C:\\Windows\\Fonts\\Arial.ttf";
	std::ifstream fontFile(fontPath);
	if (!fontFile.good()) {
		std::cerr << "[ERROR] Font file not found: " << fontPath << std::endl;
		return;
	}
	fontFile.close();

	try {
		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 1;
		config.GlyphExtraSpacing.x = 1.0f;

		// Check if ImGui font atlas is valid before adding fonts
		if (ImGui::GetIO().Fonts) {
			radarFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.c_str(), 12, &config);
			if (!radarFont) {
				std::cerr << "[ERROR] Failed to load radar font from: " << fontPath << std::endl;
			}

			// add small font
			ImFontConfig configSmall;
			configSmall.SizePixels = 12.0f; // reduce size?
			tableFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.c_str(), 14, &configSmall);
			if (!tableFont) {
				std::cerr << "[ERROR] Failed to load table font from: " << fontPath << std::endl;
			}
		} else {
			std::cerr << "[ERROR] ImGui font atlas is null, cannot load fonts." << std::endl;
		}
	} catch (const std::exception& e) {
		std::cerr << "[ERROR] Exception in createFonts: " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "[ERROR] Unknown exception in createFonts." << std::endl;
	}
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
