/**
 * Handles all economy of bot. \n
 * Contains all production facilities, handles issuing commands to them. \n
 * This class will be later extended to be able to handle expansion of bot in game. \n
 */
#pragma once

#include <windows.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "Shared.h"
#include "WorkerManager.h"
#include "ArmyManager.h"
#include "DataManager.h"


#include <queue>

#define MAX_WORKERS 30

class BuildManager{
public:
	BuildManager();
	~BuildManager();
	
	void onFrame();
	
	void drawDebug();

	bool add(BWAPI::Unit* unit);

	BWAPI::TilePosition getBuildPosition(BWAPI::TilePosition aroundTile, BWAPI::UnitType uType);
private:
	/** Experimental, repeat currently unused */
	struct buildCommand {
		BWAPI::UnitType uType;
		int repeat;
	};
	
	std::set<BWAPI::Unit*> barracks;
	std::set<BWAPI::Unit*> commandCenters;// ked budem pracovat s expansions, zatial mam len jeden command center
	// treba pridat este tie ostatne budovi
	std::set<BWAPI::Unit*> factories; // zatial nepouzivam pouzijem opening zrejme zmenim na 2x baracs 2x factory ? buildovat 2x tanky 1x medic 1x marine uvidim ked zistim costy
	std::set<BWAPI::Unit*> starports; // postavim kvoli science vessel aby som mohol mat detectora // zatial nepouzivam

	BWAPI::Unit* scienceFacility; // kvoli science vessel // zatial nepouzivam
	BWAPI::Unit* academy; // postavim po 2 barracks 

	int supplyQueued;
	int vesselsQueued;
	//int starportsQueued;
	int barracksQueued;
	bool academyQueued;
	bool scienceQueued;
	bool nextPlace;
	void checkWorkers();
	void checkSupply();
	int supplyNeeded();
	void checkArmy();
	bool addOnHandler(BWAPI::UnitType uType);
	bool buildingHandler(BWAPI::UnitType uType);
	bool unitHandler(BWAPI::UnitType uType);

	void checkBuildings();
	int barrackCount();
	int starportCount();
	BWAPI::UnitType getArmyOrder();

	void checkRefinery();

	bool BuildManager::handleOrder(buildCommand bc);

	void executeBuildOrder();

	std::vector<buildCommand> buildOrder;
	std::vector<buildCommand> importantBuildOrder;
	std::vector<buildCommand> armyOrder;
	std::vector<BWAPI::TechType> researchOrder; // budem volat po impotantBuildOrder

};