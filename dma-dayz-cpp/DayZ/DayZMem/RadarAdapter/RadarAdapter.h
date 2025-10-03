#pragma once
#include "DMARender/includes.h"
#include "../MemoryUpdater/MemoryUpdater.h"
#include <fstream>
//#include "AimAssist/aim-assist.h"
#include "../AimAssist/aim-assist.h"

namespace DayZ {
	class RadarAdapter : public DMARender::IRadar {

		std::shared_ptr<DayZ::MemoryUpdater> memUpdater; // Shared Pointer
		std::shared_ptr<DMARender::RenderBridge> renderBridge; // Shared Pointer
		std::set<std::string> favoriteSteamIDs; // SteamIDs from textfile
		//std::unique_ptr<DayZ::AimAssist> aimAssist;

		void loadFavoriteSteamIDs(const std::string& filePath); // function to load SteamIDs from textfile
		ImFont* radarFont;
		ImFont* tableFont;
		void drawLoot(DayZ::Camera* camera, DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities);
		void drawAliveEntities(DayZ::Camera* camera, DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform, const std::vector<std::shared_ptr<DayZ::Entity>>& entities, Scoreboard* scoreboard);
		void drawPlayerList(DayZ::Camera* camera, Scoreboard* scoreboard);
		void drawServerPlayerList(std::shared_ptr<DayZ::Scoreboard> scoreboard);

		// AimAssist instance
		std::unique_ptr<DayZ::AimAssist> aimAssist;


	public:
		RadarAdapter(std::shared_ptr<DayZ::MemoryUpdater> memUpdater, std::shared_ptr<DMARender::RenderBridge> renderBridge); // Constructor
		// Inherited via IRadar
		virtual void DrawOverlay(DMARender::IGameMap* curMap, const DMARender::MapTransform& mTransform) override;
		virtual void createFonts() override;

	};
}
