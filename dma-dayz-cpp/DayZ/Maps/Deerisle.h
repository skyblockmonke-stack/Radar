#pragma once
#include "DMARender/includes.h"
namespace DayZ {
	class Deerisle : public DMARender::IGameMap {
		

	public:
		Deerisle() : DMARender::IGameMap(std::string("Deerisle"), std::string("mapcontent\\deerisle.png")) {}

		// Inherited via IGameMap
		virtual ImVec2 translatePoint(const DMARender::Vector3& gamePoint) override;
	};
}