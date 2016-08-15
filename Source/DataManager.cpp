#include "DataManager.h"

using namespace BWAPI;
/** 
 * Constructor
 */ 
DataManager::DataManager(){
	myBase = BWTA::getStartLocation(Broodwar->self());
	enemyBase = BWTA::getStartLocation(Broodwar->enemy());
	basePosition = myBase->getPosition();
	baseTilePosition = myBase->getTilePosition();
}

/**
 * Destructor
 */
DataManager::~DataManager(){
	delete myBase;
	delete enemyBase;
}

/**
 * Debug info
 */
void DataManager::drawDebug(){
	Broodwar->drawTextScreen(30, 20, "\x08 %d      %d", reservedMinerals, reservedGas);
}

/**
 * Function return amount of minerals minus reserved minerals. \n
 * Reservation is called from BuildManager
 */
int DataManager::getMinerals(){
	return Broodwar->self()->minerals() - reservedMinerals;
}

/**
 * Function return amount of gas minus reserved gas. \n
 * Reservation is called from BuildManager
 */
int DataManager::getGas(){
	return Broodwar->self()->gas() - reservedGas;
}

/**
 * reserves minerals
 * called from BuildManager
 */
bool DataManager::reserveMinerals(int amount){
	reservedMinerals += amount;
	return true;
}

/**
 * reserves gas
 * called from BuildManager
 */
bool DataManager::reserveGas(int amount){
	reservedGas += amount;
	return true;
}

/**
 * free reserved minerals
 * called from BuildManager
 */
int DataManager::freeReservedMinerals(){
	int ret = reservedMinerals;
	reservedMinerals = 0;
	return ret;
}

/**
 * free reserved gas
 * called from BuildManager
 */
int DataManager::freeReservedGas(){
	int ret = reservedGas;
	reservedGas = 0;
	return ret;
}

/**
 * Function return supplyStruct containg currently used supply and total supply plus supply that is under construction
 */
supplyStruct DataManager::getSupply(){// mam tu /2 lebo to vracia 2x to co vidim zergovia veru 0.5 ci co, mozno bude zaujimavejsie nechat to v povodnej hodnote
	supplyStruct s;
	s.total = Broodwar->self()->supplyTotal()/2 + 8*Broodwar->self()->incompleteUnitCount(UnitTypes::Terran_Supply_Depot);
	s.used = Broodwar->self()->supplyUsed()/2;
	return s;
}

/**
 * Returns position of base 
 */
Position DataManager::getBasePosition(){
	return this->basePosition;
}

/**
 * Returns tile position of base
 */

TilePosition DataManager::getBaseTilePosition(){
	return this->baseTilePosition;
}

/**
 * Returns gathering ratio which specifies how much workers should gather minerals : gas. \n
 * Currently constant 5:1
 */
int DataManager::getGatheringRatio(){
	return ratio;
}