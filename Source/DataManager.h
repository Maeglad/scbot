/**
 * Stores data about game and modifies data from game.
 */
#pragma once

#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>

#include "Shared.h"

class DataManager{
public:
	DataManager();
	~DataManager();

	int getMinerals();
	int getGas();
	BWAPI::Position getBasePosition();
	BWAPI::TilePosition getBaseTilePosition();

	supplyStruct getSupply();

	bool reserveMinerals(int amount);
	bool reserveGas(int amount);
	int freeReservedMinerals();
	int freeReservedGas();
	bool addSupply();
	bool removeSupply();
	void drawDebug();
	int getGatheringRatio();
private:
	static const bool debug = true;
	BWTA::BaseLocation* myBase;
	BWTA::BaseLocation* enemyBase;
	BWAPI::Position basePosition;
	BWAPI::TilePosition baseTilePosition;

	
	
	int reservedMinerals;
	int reservedGas;
	int plannedSupplyDepots;
	static const int ratio = 5;// momentalne nepouzivam 5:1 workery mineraly:gas
};