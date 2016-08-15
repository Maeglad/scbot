#pragma once
#include <windows.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "Shared.h"
#include "DataManager.h"
#include "SquadManager.h"
//#include "SquadManager.h"

#define CONSIDERABLE_ARMY 100 // 100 supply v army by malo stacit na utok
// utok momentalne nie je implementovany aj ked by to bol len jeden if lebo v 90% pripadoch sa vojaci zachovaju
// zle a zautocia na zle jednotky/budovi a vsetci zomru v nepriatelskej baze

class ArmyManager{
public:
	ArmyManager();
	~ArmyManager();

	BWAPI::UnitType getArmyOrder();
	bool add(BWAPI::Unit* unit);
	void onFrame();
	void drawDebug();
private:
	int medicRatio; // 4 marines on one medic
	std::set<BWAPI::Unit*> buildings;
	std::set<BWAPI::Unit*> marines;
	std::set<BWAPI::Unit*> medics;
	std::set<SquadManager*> squads;
	BWAPI::Unit* academy;
	void update();

};