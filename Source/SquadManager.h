#pragma once
#include <windows.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "Shared.h"
#include "DataManager.h"

class SquadManager{
private:
	BWAPI::Position target;
	int radius;
	std::set<BWAPI::Unit*> units;
	void update();
	void attack(BWAPI::Unit* unit);
public:
	int state;// 0 idle // 1 attacking
	SquadManager();
	~SquadManager();
	void setTarget(BWAPI::Position pos);
	void setRadius(int radius);
	SquadManager split();// probably wont use, spliting army is not realy a good tactic
	bool add(BWAPI::Unit* unit);
	bool merge(SquadManager* squad);
	std::set<BWAPI::Unit*> getUnits();
	void onFrame();
};