#pragma once
#include "DMAMemoryManagement/includes.h";
#include "InventoryItem.h"

namespace DayZ {
	struct EntityInventory : public DMAMem::MemoryObject {
		std::shared_ptr<DayZ::InventoryItem> handItem;
		bool isHandItemValid;
		EntityInventory() {
			handItem = std::shared_ptr<DayZ::InventoryItem>(new DayZ::InventoryItem());
			this->registerPointer(0xF8, handItem.get());
			this->registerOffset(0x1B0, &isHandItemValid, sizeof(bool));
		}
	};
}