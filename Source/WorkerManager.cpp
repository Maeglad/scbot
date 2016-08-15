#include "WorkerManager.h"
using namespace BWAPI;

/**
 * Constructor
 */
WorkerManager::WorkerManager(){
	this->refineries.clear();
	this->workerBuildingPositionMap.clear();
	this->gasSet = Broodwar->getGeysers();
	this->mineralSet = Broodwar->getMinerals();
	workersMap.clear();
	this->nearestGeyzer = NULL;
	this->nearestMineral = NULL;
	this->nearestRefinery = NULL;
	this->academyBuilder = NULL;
	findNearestMineral();
	this->buildingWorkers.clear();
//occupied.clear;
}

/**
 * Destructor \n
 * TODO
 */
WorkerManager::~WorkerManager(){
}

/**
 * onFrame method called from BotModule::onFrame
 */
void WorkerManager::onFrame(){
	update();
	drawDebug();
}

/**
 * Debug info
 */
void WorkerManager::drawDebug(){

	Position basePosition = dataManager->getBasePosition();
	int homeX = basePosition.x();
	int homeY = basePosition.y();
	Broodwar->drawCircleMap(homeX, homeY, 50, BWColor::RED, FALSE);
	std::set<Unit*>::iterator unitIt;
	Position unitPos;
		
	for(unitIt = mineralSet.begin(); unitIt != mineralSet.end(); ++unitIt){
		Unit* unit = (*unitIt);
		unitPos = unit->getPosition();
		Broodwar->drawLineMap(unitPos.x(), unitPos.y(), homeX, homeY, BWColor::RED);
		Broodwar->drawCircleMap(unitPos.x(), unitPos.y(), 30, BWColor::ORANGE);
	}

	for(unitIt = gasSet.begin(); unitIt != gasSet.end(); ++unitIt){
		Unit* unit = (*unitIt);
		unitPos = unit->getPosition();
		Broodwar->drawLineMap(unitPos.x(), unitPos.y(), homeX, homeY, BWColor::GREEN);
		Broodwar->drawCircleMap(unitPos.x(), unitPos.y(), 30, BWColor::YELLOW);
	}
	std::map<Unit*, workerState>::iterator it;
	for(it = workersMap.begin(); it != workersMap.end(); ++it){
		Unit* unit = it->first;
		if(it->second == mineral){
			unitPos = unit->getPosition();
			Broodwar->drawCircleMap(unitPos.x(), unitPos.y(), 12, BWColor::BLUE, true);
			Position targetPos = unit->getTargetPosition();
			Broodwar->drawLineMap(unitPos.x(), unitPos.y(), targetPos.x(), targetPos.y(), BWColor::BLUE);
			

		}
		else if(it->second == gas){
			unitPos = unit->getPosition();
			Broodwar->drawCircleMap(unitPos.x(), unitPos.y(), 12, BWColor::GREEN, true);
			Position targetPos = unit->getTargetPosition();
			Broodwar->drawLineMap(unitPos.x(), unitPos.y(), targetPos.x(), targetPos.y(), BWColor::GREEN);
			
		}
		else if(it->second == idle){
			unitPos = unit->getPosition();
			Broodwar->drawCircleMap(unitPos.x(), unitPos.y(), 12, BWColor::GREY, true);
		}

	}
	if(nearestMineral != NULL){
		Broodwar->drawCircleMap(nearestMineral->getPosition().x(), nearestMineral->getPosition().y(), 20, BWColor::PURPLE, true);
	}

	if(nearestGeyzer != NULL){
		Broodwar->drawCircleMap(nearestGeyzer->getPosition().x(), nearestGeyzer->getPosition().y(), 20, BWColor::PURPLE, true);
	}

	if(nearestRefinery != NULL){
		Broodwar->drawCircleMap(nearestRefinery->getPosition().x(), nearestRefinery->getPosition().y(), 20, BWColor::RED, true);
	}
	//Broodwar->sendText("gasSet %d", gasSet.size());
	if(beingBuild(UnitTypes::Terran_Supply_Depot)){
		//Broodwar->sendText("Stavam supply");
	}
	if(academyBuilder != NULL){
		unitPos = academyBuilder->getPosition();
		Broodwar->drawCircleMap(unitPos.x(), unitPos.y(), 12, BWColor::ORANGE, true);
	}
	if(beingBuild(UnitTypes::Terran_Academy)){
		Broodwar->sendText("Building academy");
	}
	
}

/**
 * Finds nearest Vespene Geyzer to base position. \n
 * TODO add specific position to finder
 */
void WorkerManager::findNearestGeyzer(){
		Position basePosition = dataManager->getBasePosition();	
		double dist = basePosition.getDistance( (*gasSet.begin())->getPosition() );
		nearestGeyzer = (*gasSet.begin());
		std::set<BWAPI::Unit*>::iterator unitIt;
		
		for(unitIt = gasSet.begin(); unitIt != gasSet.end(); ++unitIt){

			BWAPI::Unit* gas = (*unitIt);
			double tdist = basePosition.getDistance(gas->getPosition());
			
			if(tdist < dist){ 
				nearestGeyzer = gas; 
				dist = tdist;
			}
			
		}
};

/**
 * Finds nearest mineral to base position. \n
 * TODO add specific position to finder
 */
void WorkerManager::findNearestMineral(){
		Position basePosition = dataManager->getBasePosition();
		double dist = basePosition.getDistance( (*mineralSet.begin())->getPosition() );
		std::set<BWAPI::Unit*>::iterator unitIt;

		for(unitIt = mineralSet.begin(); unitIt != mineralSet.end(); ++unitIt){
			
			BWAPI::Unit* mineral = (*unitIt);
			double tdist = basePosition.getDistance(mineral->getPosition());
			
			if(tdist < dist){ 
				nearestMineral = mineral; 
				dist = tdist;
			}
		}
}

/**
 * Finds nearest refinery to base position. \n
 * TODO add specific position to finder
 */
void WorkerManager::findNearestRefinery(){
	Unit* unit = NULL;
	double curDist = MAXINT;
	Position basePosition = dataManager->getBasePosition();
	for(std::set<Unit*>::iterator it = refineries.begin(); it != refineries.end(); ++it){
		double tmpDist = basePosition.getDistance((*it)->getPosition());
		if(tmpDist < curDist){
			curDist = tmpDist;
			unit = (*it);
		}
	}
	nearestRefinery = unit;
}

/**
 * Checks if refinery exists
 */
bool WorkerManager::hasRefinery(){
	if(beingBuild(UnitTypes::Terran_Refinery)){return true;}
	if(nearestRefinery != NULL){return true;}
	return false;
}

/**
 * Return how many available workers bot have
 */
int WorkerManager::workerCount(){
	return workersMap.size();
}

/**
 * Return pointer on nearestGeyzer
 */
BWAPI::Unit* WorkerManager::getNearestGeyzer(){
	return this->nearestGeyzer;	
}

/**
 * Checks if all resource sets have only valid units. \n
 * Checks if nearest resource is found if not finds its. \n
 * calls WorkerManager::checkWork
 * Called from WorkerManager::onFrame 
 */
void WorkerManager::update(){ // moze padat ak to invalidatene iterator treba skusit a opravit ak hej
	std::map<BWAPI::Unit*, workerState>::iterator unitIt = workersMap.begin();

	while(unitIt != workersMap.end()) {
    // copy the current iterator then increment it
		std::map<BWAPI::Unit*, workerState>::iterator current = unitIt;
		++unitIt;
		//int n = *current;
		if ((current->first)->getHitPoints() <= 0) {
        // don't invalidate iterator it, because it is already
        // pointing to the next element
			workersMap.erase(current);
			Broodwar->sendText("Removing worker");
		}
	}

	std::set<Unit*>::iterator it = mineralSet.begin();

	while(it != mineralSet.end()) {
    // copy the current iterator then increment it
		std::set<BWAPI::Unit*>::iterator current = it;
		++it;
		//int n = *current;
		if((*current)->isVisible() == false){
			continue;
		}
		if ((*current)->getResources() == 0) {
        // don't invalidate iterator it, because it is already
        // pointing to the next element
			mineralSet.erase(current);
		}
	}

	it = gasSet.begin();
	while(it != gasSet.end()) {
    // copy the current iterator then increment it
		std::set<BWAPI::Unit*>::iterator current = it;
		++it;
		//int n = *current;
		if((*current)->isVisible() == false){
			continue;
		}
		if ((*current)->getResources() == 0) {
        // don't invalidate iterator it, because it is already
        // pointing to the next element
			gasSet.erase(current);
		}
	}

	it = refineries.begin();
	while(it != refineries.end()) {
    // copy the current iterator then increment it
		std::set<BWAPI::Unit*>::iterator current = it;
		++it;
		//int n = *current;
		if ((*current)->getResources() == 0) {
        // don't invalidate iterator it, because it is already
        // pointing to the next element
			refineries.erase(current);
		}
	}

//	gas az tak nepouzivam nehovoriac o tom ze refinery sa da zbierat aj depleted
	
	if(nearestGeyzer == NULL){
		findNearestGeyzer();
	}
	if(nearestMineral == NULL){
		findNearestMineral();
	}
	
	if(nearestGeyzer->getResources() == 0){
		findNearestGeyzer();
	}
	if(nearestMineral->getResources() == 0){
		findNearestMineral();
	}

	if(nearestRefinery == NULL){
		findNearestRefinery();
	}

	checkWork();
}

/**
 * Finds worker that is currently gathering mineral updates workerBuildingPositionMap and updates his workerState
 * Called from BuildManager::handleOrder
 */
bool WorkerManager::buildOrder(TilePosition buildPosition, UnitType uType){
	update();
	if(uType == UnitTypes::Terran_Academy){
		Broodwar->sendText("ACADEMY");
	}
	std::map<Unit*, workerState>::iterator it;
	it = workersMap.begin();
	while(it != workersMap.end()){
		workerState state = it->second;
		Unit* unit = it->first;
		if((state == mineral)&&(unit->isConstructing() == false)){
			if(it->first->build(buildPosition, uType)){
				it->second = building;
				std::pair<UnitType,TilePosition> buildingTilePosition;
				buildingTilePosition.first = uType;
				buildingTilePosition.second = buildPosition;
				//occupied.insert(buildPosition);
				Unit* unit = it->first;
				workerBuildingPositionMap[unit] = buildingTilePosition;
				if(uType == UnitTypes::Terran_Refinery){
					refineryBuilder = unit;
				}
				if(uType == UnitTypes::Terran_Academy){
					academyBuilder = unit;
				}
				return true;
			}
			

			return false; 
		}
		++it;
	}
	return false;
	
	
}

/**
 * When building has been constructed this method is called to free worker from workerBuildingPositionMap and worker is again assigned gathering work
 * Called from BuildManager::add
 */
bool WorkerManager::freeBuildOrder(BWAPI::TilePosition buildPosition, BWAPI::UnitType uType){
	std::map<Unit*, std::pair<UnitType,TilePosition> >::iterator it;
	for(it = workerBuildingPositionMap.begin(); it != workerBuildingPositionMap.end(); ++it){
		Unit* unit = it->first;
		std::pair<UnitType,TilePosition> buildingTilePosition = it->second;
		if((buildingTilePosition.first == uType)&&(buildingTilePosition.second == buildPosition)){
			workersMap[unit] = idle;
			workerBuildingPositionMap.erase(unit);
			Broodwar->sendText("freeing build order on %s", uType.c_str());
			return true;
		}
	}
	Broodwar->sendText("failed to free %s", uType.c_str());
	return false;
}

/**
 * Returns count of buildings of uType being built
 */
int WorkerManager::beingBuild(UnitType uType){
	int ret = 0;
	std::map<Unit*, std::pair<UnitType,TilePosition>>::iterator it;
	for( it = workerBuildingPositionMap.begin(); it != workerBuildingPositionMap.end(); it++){
		std::pair<UnitType,TilePosition> buildingTilePosition = it->second;
		if(buildingTilePosition.first == uType) ret += 1;
	}

	return ret;
}

/**
 * Handles adding new units 
 *  - mineral fields
 *  - gas fields
 *  - refineries
 *  - workers
 * Called from BotModule::onUnitShow \n
 * Called from BotModule::onUnitMorph 
 */
bool WorkerManager::add(BWAPI::Unit* unit){
	BWAPI::UnitType uType = unit->getType();
			
	if(uType.isMineralField()){
		mineralSet.insert(unit);
		//Broodwar->sendText("Adding mineral field");
		return true;
	}
	
	if(uType == BWAPI::UnitTypes::Resource_Vespene_Geyser){
		gasSet.insert(unit);
		Broodwar->sendText("Adding vespene geyser");
		return true;
	}
	
	if(Broodwar->self()->isEnemy(unit->getPlayer())){
		return false;
	}
	
	if(uType == UnitTypes::Terran_Refinery){
		return addRefinery(unit);
	}

	if(uType == BWAPI::UnitTypes::Terran_SCV){
		workersMap.insert(std::pair<Unit*, workerState>(unit,idle));
		//Broodwar->sendText("Adding worker terran");
		return true;
	}
	if(uType == BWAPI::UnitTypes::Protoss_Probe){
		workersMap.insert(std::pair<Unit*, workerState>(unit,idle));
		Broodwar->sendText("Adding worker protoss");
		return true;
	}
	if(uType == BWAPI::UnitTypes::Zerg_Drone){
		workersMap.insert(std::pair<Unit*, workerState>(unit,idle));
		Broodwar->sendText("Adding worker zerg");
		return true;
	}
	return false;
}

/**
 * Adds refinery to refineries set and display debug info
 * Called from WorkerManager::add
 */
bool WorkerManager::addRefinery(Unit* unit){
	if(Broodwar->self()->isEnemy(unit->getPlayer())){return false;}
	if(unit->getType() == UnitTypes::Terran_Refinery){
		refineries.insert(unit);
		Broodwar->sendText("Adding refinery");
		return true;

	}
	
	return false;
}

/**
 * Checks if all units in workersMap are alive, if not erases them. \n
 * Checks if buildings in workerBuildingPositionMap are being constructed, if not checks if building can be built and if not requests new location
 * Manages distribution of workers to minerals and refineries (partialy done)
 */
void WorkerManager::checkWork(){
	std::map<BWAPI::Unit*, workerState>::iterator unitIt;
	// just in case
	for(unitIt = workersMap.begin(); unitIt != workersMap.end();++unitIt){
		if((unitIt->first)->getHitPoints() <= 0){
			// jednotka je mrtva ale build order nebol vykonany
			Unit* unit = unitIt->first;
			workersMap.erase(unitIt);
			if(workerBuildingPositionMap.find(unit) != workerBuildingPositionMap.end()){
				std::pair<UnitType, TilePosition> buildingTilePosition;
				buildingTilePosition = workerBuildingPositionMap[unit];
				buildOrder(buildingTilePosition.second, buildingTilePosition.first);
				workerBuildingPositionMap.erase(unit);
			}
			
			return;
		}
		
		if(refineryBuilder != NULL){
			if(refineryBuilder->isGatheringGas()){
				workersMap[refineryBuilder] = idle;
				refineryBuilder = NULL;
			}
		}

		if(workerBuildingPositionMap.find(unitIt->first) != workerBuildingPositionMap.end()){
			unitIt->second = idle;
		}

		
		if(unitIt->first->isIdle()){unitIt->second = idle;}
		if( unitIt->second == idle ){
			Unit* unit = unitIt->first;
			if(workerBuildingPositionMap.find(unit) != workerBuildingPositionMap.end()){
				std::pair<UnitType,TilePosition> buildingTilePosition = workerBuildingPositionMap[unit];
				if(Broodwar->canBuildHere(unit,buildingTilePosition.second, buildingTilePosition.first)){
					unit->build(buildingTilePosition.second, buildingTilePosition.first);
					unitIt->second = building;
				}
				else{
					buildingTilePosition.second = buildManager->getBuildPosition(dataManager->getBaseTilePosition(), buildingTilePosition.first);
					workerBuildingPositionMap[unit] = buildingTilePosition;
				}
				
			}
			else{
				int ratio = dataManager->getGatheringRatio();
				if(nearestRefinery != NULL){
					int mineralGathers = mineralGatherers();
					int gasGathers = gasGatherers();
					if((mineralGathers / gasGathers) > ratio){
						(unitIt->first)->gather(nearestRefinery);
						unitIt->second = workerState::gas;
					}
					else{
						(unitIt->first)->gather(nearestMineral);
						unitIt->second = workerState::mineral;
					}


				}
				else{
					(unitIt->first)->gather(nearestMineral);
					unitIt->second = workerState::mineral;
				}
			}
		}
		
	}
}

/**
 * Returns how many workers are gathering minerals
 */
int WorkerManager::mineralGatherers(){
	std::map<Unit*, workerState>::iterator it;
	int ret = 1;
	for(it = workersMap.begin(); it != workersMap.end(); ++it){
		if(it->second == workerState::mineral)ret++;
	}
	return ret;
}

/**
 * Returns how many workers are gathering gas
 */
int WorkerManager::gasGatherers(){
	std::map<Unit*, workerState>::iterator it;
	int ret = 1;
	for(it = workersMap.begin(); it != workersMap.end(); ++it){
		if(it->second == workerState::gas)ret++;
	}
	return ret;
}