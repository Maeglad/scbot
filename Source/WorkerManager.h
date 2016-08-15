/**
 * Handles worker managing
 *  - assigning workers to mineral fields(partialy done)
 *  - assigning workers to refineries (partialy done)
 *  - finding nearest rescourses
 */
#pragma once

#include <windows.h>

#include <BWAPI.h>
#include <BWTA.h>

#include <cstdlib>
#include <string>
#include <set>

#include "Shared.h"
#include "DataManager.h"
#include "BuildManager.h"


class WorkerManager{
private:
	enum workerState{
		building,
		gas,
		mineral,
		scouting,
		idle
	};

	void findNearestMineral();
	void findNearestGeyzer();
	void findNearestRefinery();
	std::set<BWAPI::Unit*> mineralSet;
	std::set<BWAPI::Unit*> refineries;
	std::set<BWAPI::Unit*> gasSet;
	
	BWAPI::Unit* nearestMineral;
	BWAPI::Unit* nearestGeyzer;
	BWAPI::Unit* nearestRefinery;
	std::set<BWAPI::Unit*> buildingWorkers;
	BWAPI::Unit* refineryBuilder;
	BWAPI::Unit* academyBuilder;
	int mineralGatherers();
	int gasGatherers();

	std::map<BWAPI::Unit*, std::pair<BWAPI::UnitType,BWAPI::TilePosition>> workerBuildingPositionMap;
	std::map<BWAPI::Unit*, workerState> workersMap;
public:
	
	//std::set<BWAPI::Position> occupied;
	WorkerManager();
	~WorkerManager();

	void onFrame();

	void drawDebug();

	int workerCount();

	bool add(BWAPI::Unit* unit);
	bool addRefinery(BWAPI::Unit* unit);
	bool remove(BWAPI::Unit* unit);
	void update();
	void checkWork();
	//void setBasePosition(BWAPI::Position pos);
	bool buildOrder(BWAPI::TilePosition buildPosition, BWAPI::UnitType uType);
	bool freeBuildOrder(BWAPI::TilePosition buildPosition, BWAPI::UnitType uType);
	bool hasRefinery();
	int beingBuild(BWAPI::UnitType uType);
	
	BWAPI::Position getBasePosition();
	BWAPI::Unit* getNearestGeyzer();


};