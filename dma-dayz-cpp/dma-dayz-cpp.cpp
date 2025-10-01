// dma-dayz-cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include "DMAMemoryManagement/includes.h"
#include "DayZ/DayZMem/DayZMem.h"
#include "DayZ/Structs/NetworkManager.h";
#include "DayZ/Structs/WorldPointer.h";
#include "DayZ/DayZMem/MemoryUpdater/MemoryUpdater.h"
#include "DayZ/DayZMem/OverlayAdapter/OverlayAdapter.h"
#include "DayZ/DayZMem/RadarAdapter/RadarAdapter.h"
#include "DMARender/includes.h"
#include "DayZ/Maps/ChernarusPlus.h"
#include "DayZ/Maps/Namalsk.h"
#include "DayZ/Maps/Livonia.h"
#include "DayZ/Maps/DeadFall.h"
#include "DayZ/Maps/Alteria.h"
#include "DayZ/Maps/Lux.h"
#include "DayZ/Maps/Esseker.h"
#include "DayZ/Maps/Deerisle.h"
#include "DayZ/Maps/Sakhal.h"
#include "DayZ/Maps/Takistan.h"
#include "DayZ/Maps/Banov.h"

DMAMem::VmmManager* createVmmManager() {
    __try {
        return new DMAMem::VmmManager();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        std::cerr << "[ERROR] SEH exception in VmmManager initialization." << std::endl;
        return nullptr;
    }
}

int main()
{
    std::cout << "[DEBUG] Program started." << std::endl;

    std::cout << "[DEBUG] Starting VmmManager initialization..." << std::endl;
    auto vmm = createVmmManager();
    if (!vmm) {
        std::cerr << "[ERROR] Failed to create VmmManager." << std::endl;
        return -1;
    }
    std::cout << "[DEBUG] VmmManager initialized successfully." << std::endl;

    try {
        std::cout << "[DEBUG] Starting DayZ::Mem initialization..." << std::endl;
        auto game = new DayZ::Mem(vmm);
        std::cout << "[DEBUG] DayZ::Mem initialized successfully." << std::endl;

        std::cout << "[DEBUG] Starting MemoryUpdater initialization..." << std::endl;
        auto memUpdater = std::shared_ptr<DayZ::MemoryUpdater>(new DayZ::MemoryUpdater(game));
        memUpdater->beginUpdateLoop();
        std::cout << "[DEBUG] MemoryUpdater initialized and loop started." << std::endl;

        std::cout << "[DEBUG] Starting RenderWindow initialization..." << std::endl;
        DMARender::RenderWindow cmd = DMARender::RenderWindow();
        std::cout << "[DEBUG] RenderWindow initialized successfully." << std::endl;

        std::cout << "[DEBUG] Setting up overlay and maps..." << std::endl;
        cmd.getBridge()->setOverlay(std::make_shared<DayZ::OverlayAdapter>(memUpdater, cmd.getBridge()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::ChernarusPlus>(new DayZ::ChernarusPlus()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Namalsk>(new DayZ::Namalsk()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Livonia>(new DayZ::Livonia()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::DeadFall>(new DayZ::DeadFall()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Alteria>(new DayZ::Alteria()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Lux>(new DayZ::Lux()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Esseker>(new DayZ::Esseker()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Deerisle>(new DayZ::Deerisle()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Sakhal>(new DayZ::Sakhal()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Takistan>(new DayZ::Takistan()));
        cmd.getBridge()->addMap(std::shared_ptr<DayZ::Banov>(new DayZ::Banov()));
        std::cout << "[DEBUG] Overlay and maps set up successfully." << std::endl;

        std::cout << "[DEBUG] Creating RadarAdapter..." << std::endl;
        auto radarAdapter = std::make_shared<DayZ::RadarAdapter>(memUpdater, cmd.getBridge());
        std::cout << "[DEBUG] Creating fonts..." << std::endl;
        radarAdapter->createFonts();
        cmd.getBridge()->setRadar(radarAdapter);
        std::cout << "[DEBUG] RadarAdapter set up successfully." << std::endl;

        std::cout << "[DEBUG] Starting graphics thread..." << std::endl;
        std::thread gfxThread(&DMARender::RenderWindow::initializeWindow, &cmd);

        std::cout << "[DEBUG] Joining graphics thread..." << std::endl;
        gfxThread.join();
        memUpdater->endUpdateLoop();
        std::cout << "[DEBUG] Program ended successfully." << std::endl;

        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception caught: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "[ERROR] Unknown exception caught." << std::endl;
        return -1;
    }
}
