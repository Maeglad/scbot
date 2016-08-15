#include "BuildManager.h"
using namespace BWAPI;

/**
 * Constructor
 */
BuildManager::BuildManager(){
	buildOrder.clear();
	importantBuildOrder.clear();
	armyOrder.clear();
	barracks.clear();
	factories.clear();
	starports.clear();
	scienceFacility = NULL;
	academy = NULL;
	academyQueued = false;
	supplyQueued = 0;
	vesselsQueued = 0;
	barracksQueued = 0;
	nextPlace = false;
}

/**
 * Destructor \n
 * TODO
 */
BuildManager::~BuildManager(){
}


/**
 * onFrame method of BuildManager. \n
 * Called from BotModule::onFrame
 */
void BuildManager::onFrame(){
	checkRefinery();
	checkArmy();
	checkWorkers();
	checkSupply();
	drawDebug();
	executeBuildOrder();
}

/**
 * Checks if number of current workers does not exceed MAX_WORKERS constant and
 * if it does not orders training of new ones
 */
void BuildManager::checkWorkers(){
	if(workerManager->workerCount() > MAX_WORKERS){return;}
	else if((commandCenters.size() > 0)&&
		(buildOrder.empty() == true)){

			buildCommand bc;
			bc.repeat = 0;
			bc.uType = UnitTypes::Terran_SCV;
			buildOrder.push_back(bc);

	}
}

/**
 * Checks if current supply is sufficient and if not orders construction of new supply depot
 */
void BuildManager::checkSupply(){
	

	supplyStruct sS = dataManager->getSupply();
	
	sS.total += 8*workerManager->beingBuild(UnitTypes::Terran_Supply_Depot) + 8*supplyQueued;
	if(sS.total - sS.used <= 3*supplyNeeded()){
		supplyQueued++;
		buildCommand bc;
		bc.uType = UnitTypes::Terran_Supply_Depot;
		bc.repeat = 0;
		importantBuildOrder.insert(importantBuildOrder.begin(),bc);
		Broodwar->sendText("supply");
	}
		
}

/**
 * Computes minimum supply needed based on how many Command Centers, Barracs, etc bot have
 */
int BuildManager::supplyNeeded(){
	return commandCenters.size() + barracks.size();
}

/**
 * Checks if there are idle military production structures and if there are, requests new units
 */
void BuildManager::checkArmy(){
	checkBuildings();
	if(armyOrder.empty() == false){return;}
	for(std::set<Unit*>::iterator it = barracks.begin(); it != barracks.end(); ++it){
		if( ((*it)->isTraining() == false) && (*it)->isBeingConstructed() == false ){
			buildCommand bc;
			bc.uType = armyManager->getArmyOrder();
			bc.repeat = 0;
			armyOrder.push_back(bc);
		}
	}
}

/**
 * Checks if barracs are still alive and orders construction of new ones if requirements are met
 */
void BuildManager::checkBuildings(){
	for(std::set<Unit*>::iterator it = barracks.begin(); it != barracks.end(); ++it){
		if((*it)->getHitPoints() <= 0){
			barracks.erase(it);
			return;
		}
	}
	if(barrackCount() >= 1){ // chcem uz mat academy a stavat medicov k marineom
		if((academy == NULL)&&(academyQueued == false)&&(workerManager->beingBuild(UnitTypes::Terran_Academy) == false)){
			buildCommand bc;
			bc.repeat = 0;
			bc.uType = UnitTypes::Terran_Academy;
			importantBuildOrder.push_back(bc);
			Broodwar->sendText("academy");
			academyQueued = true;
			return;
		}
	}
	if(barrackCount() >= 3){
/*		if(starportCount() >= 1){
			if((scienceFacility != NULL)&&(scienceQueued == false)){// addded
				return;
			}
			else{
				buildCommand bc;
				bc.repeat = 0;
				bc.uType = UnitTypes::Terran_Science_Facility;
				importantBuildOrder.push_back(bc);
				scienceQueued = true;
				return;
			}
		}*/
		//buildCommand bc;
		//bc.uType = UnitTypes::Terran_Starport;
		//bc.repeat = 0;
		//importantBuildOrder.push_back(bc);
		return;
	}
	supplyStruct sS = dataManager->getSupply();
	if(barrackCount() < (sS.used/10)){
		buildCommand bc;
		bc.uType = UnitTypes::Terran_Barracks;
		bc.repeat = 0;
		importantBuildOrder.push_back(bc);
		Broodwar->sendText("Barracks");
		barracksQueued++;
	}
}

/**
 * Function return number of currently available barracks
 */
int BuildManager::barrackCount(){
	int ret = barracks.size();
	ret += workerManager->beingBuild(UnitTypes::Terran_Barracks) + barracksQueued;
	return ret;
}

int BuildManager::starportCount(){// added
	int ret = starports.size();
	ret += workerManager->beingBuild(UnitTypes::Terran_Starport);
	return ret;
}


/**
 * Checks if refinery has already been constructed and if not and requirements are met orders construction
 */
void BuildManager::checkRefinery(){
	if(workerManager->hasRefinery() == false){
		supplyStruct sS = dataManager->getSupply();
		if(sS.used >= 6){
			buildCommand bc;
			bc.repeat = 0;
			bc.uType = UnitTypes::Terran_Refinery;
			importantBuildOrder.push_back(bc);
			Broodwar->sendText("refinery");
		}
	}
}

/**
 * Checks if there are orders in order vectors. \n
 * Starts with importantBuildOrder
 *  - most important orders, needs to be carried out immediately
 * Continues buildOrder
 *  - less important orders
 * then armyOrder
 *  - contains army units that should be built
 * then researchOrder
 *  - currently unimplemented
 *  - researchOrder will be moved to separate method and will be called when needed
 */
void BuildManager::executeBuildOrder(){
	
	dataManager->freeReservedGas();
	dataManager->freeReservedMinerals();
	if(importantBuildOrder.empty() == false){
		buildCommand bc = importantBuildOrder.front();
		if(handleOrder(bc)){
			if(bc.repeat != 0){
				bc.repeat -= 1;
				importantBuildOrder.push_back(bc);
			}
			importantBuildOrder.erase(importantBuildOrder.begin());
		}
		else{
			dataManager->reserveMinerals(bc.uType.mineralPrice());
			dataManager->reserveGas(bc.uType.gasPrice());
		}
	}
	
	if(buildOrder.empty() == false){
		buildCommand bc = buildOrder.front();
		if(handleOrder(bc)){
			
			buildOrder.erase(buildOrder.begin());
			if(bc.repeat != 0){
				Broodwar->sendText("repeat %d x %s", bc.repeat,bc.uType.c_str());
				bc.repeat -= 1;
				buildOrder.push_back(bc);
			}
			
		}
		else{
			buildOrder.push_back(bc);
			buildOrder.erase(buildOrder.begin());
		}
	}

	// armyOrder

	if(armyOrder.empty() == false){
		buildCommand bc = armyOrder.front();
		//Broodwar->sendText("%s", bc.uType.c_str());
		//armyOrder.erase(armyOrder.begin());
	
		if(handleOrder(bc)){
			
			armyOrder.erase(armyOrder.begin());
			if(bc.repeat != 0){
				Broodwar->sendText("repeat %d x %s", bc.repeat,bc.uType.c_str());
				bc.repeat -= 1;
				armyOrder.push_back(bc);
			}
			
			return;
		}
		else{
			//armyOrder.push_back(bc);
			armyOrder.erase(armyOrder.begin());
		}
	}

	// tech ma vlastny order handler

	if(researchOrder.empty() == false){
		// zatial ani neviem co skumat, kedy, preco
		// ked to budem vediet pridam handler
	}
}

bool BuildManager::addOnHandler(UnitType uType){
//addon handler // stale neviem co to je addon
//ako pri techu nemam potuchy co to je
// predpoklad su to dake globalne "spelly"
// scanner bude ovladany z armymanagera nuclear silo tiez
	if( (uType == UnitTypes::Terran_Comsat_Station) || 
		(uType == UnitTypes::Terran_Nuclear_Silo) ){
			for(std::set<Unit*>::iterator it = commandCenters.begin(); it != commandCenters.end(); ++it){
				if( (*it)->getAddon() == NULL ){
					if( (*it)->isTraining() ){
						(*it)->cancelTrain();
					}
					(*it)->buildAddon(uType);
					return true;
				}
			}
		return false;
	}

	if(uType == UnitTypes::Terran_Machine_Shop){
		for(std::set<Unit*>::iterator it = factories.begin(); it != factories.end(); ++it){
			if( (*it)->getAddon() == NULL ){
				if( (*it)->isTraining() ){
					(*it)->cancelTrain();
				}
				(*it)->buildAddon(uType);
				return true;
			}
		}
		return false;
	}	
				
	if(uType == UnitTypes::Terran_Control_Tower){
		for(std::set<Unit*>::iterator it = starports.begin(); it != starports.end(); ++it){
			if( (*it)->getAddon() == NULL ){
				if( (*it)->isTraining() ){
					(*it)->cancelTrain();
				}
				(*it)->buildAddon(uType);
				return true;
			}
		}
		return false;
	}

	if((uType == UnitTypes::Terran_Physics_Lab)||
		(uType == UnitTypes::Terran_Covert_Ops)){
	
		if( scienceFacility->getAddon()->getType() != uType ){
			if( scienceFacility->isResearching() ){
				scienceFacility->cancelResearch();
			}
			scienceFacility->buildAddon(uType);
			return true;
		}
		return false;
	}		
}

bool BuildManager::buildingHandler(UnitType uType){
	// building handler tento funguje reliably treba viac testov
	TilePosition tP = getBuildPosition(dataManager->getBaseTilePosition(), uType);
	if(tP != TilePositions::None){
		if(workerManager->buildOrder(tP, uType)){
			if(uType == UnitTypes::Terran_Supply_Depot){
				supplyQueued--;
				Broodwar->sendText("supply removed");
			}
			if(uType == UnitTypes::Terran_Academy){
				academyQueued = false;
			}
			if(uType == UnitTypes::Terran_Barracks){
				barracksQueued--;
			}
			return true;
		}
	}
	return false;
}

bool BuildManager::unitHandler(UnitType uType){
	// unit handler
	if(uType == UnitTypes::Terran_SCV){
		for(std::set<Unit*>::iterator it = commandCenters.begin(); it != commandCenters.end(); ++it){
			if((*it)->isBeingConstructed() == false){
				if((*it)->isTraining() == false){
					return (*it)->train(uType);
				}
			}
		}
	}

	if(uType == UnitTypes::Terran_Marine){
		for(std::set<Unit*>::iterator it = barracks.begin(); it != barracks.end(); ++it){
			if((*it)->isBeingConstructed() == false){
				if((*it)->isTraining() == false){
					return (*it)->train(uType);
				}
			}
		}
	}

	if(uType == UnitTypes::Terran_Medic){
		for(std::set<Unit*>::iterator it = barracks.begin(); it != barracks.end(); ++it){
			if((*it)->isBeingConstructed() == false){
				if((*it)->isTraining() == false){
					return (*it)->train(uType);
				}
			}
		}
	}

	if(uType == UnitTypes::Terran_Science_Vessel){
		for(std::set<Unit*>::iterator it = starports.begin(); it != starports.end(); ++it){
			if((*it)->isBeingConstructed() == false){
				if((*it)->isTraining() == false){
					vesselsQueued++;
					return (*it)->train(uType);
				}
			}
		}
	}
	return false;
}

/**
 * Handles order execution. \n
 * Will be split when all handlers will be completed
 */ 
bool BuildManager::handleOrder(buildCommand bc){
	if((dataManager->getMinerals() >= bc.uType.mineralPrice())&&
		(dataManager->getGas() >= bc.uType.gasPrice())&&
		(Broodwar->canMake(NULL,bc.uType)))
	{
		if(bc.uType.isAddon()){
			return addOnHandler(bc.uType);
		}

		if(bc.uType.isBuilding()){
			return buildingHandler(bc.uType);
		}

		return unitHandler(bc.uType);

	}
	return false;
}

/**
 * on top left draws \n
 * first line: importantBuildOrder size, then from datamanager - reserved mins and gas
 * second line: if academy is quued 1 else 0
 * third line: how many supply depots are queued
 */
void BuildManager::drawDebug(){
	Broodwar->drawTextScreen(10, 20, "\x08 %d", importantBuildOrder.size());
	Broodwar->drawTextScreen(10, 40, "\x08 %d", academyQueued);
	Broodwar->drawTextScreen(10, 60, "\x08 %d", supplyQueued);
}

/**
 * Handles adding a new unit. \n
 * Called form BotModule
 */
bool BuildManager::add(Unit* unit){
	// if enemy
	if(Broodwar->self()->isEnemy(unit->getPlayer())){return false;}
	UnitType uType = unit->getType();

	// if its not building lets not waste time
	if(uType.isBuilding() == false){return false;}

	if(workerManager->beingBuild(uType)){
		workerManager->freeBuildOrder(unit->getTilePosition(), unit->getType());
	}

	if(uType == UnitTypes::Terran_Supply_Depot){
		Broodwar->sendText("Supply depot constructed");
		//supplyQueued--;
		return true;
	}
	if(uType == UnitTypes::Terran_Command_Center){
		commandCenters.insert(unit);
		Broodwar->sendText("Adding Terran Command Center");
		return true;
	}
	if(uType == UnitTypes::Terran_Academy){
		academy = unit;
		academyQueued = false;
		return true;
	}

	if(uType == UnitTypes::Terran_Barracks){
		barracks.insert(unit);
		Broodwar->sendText("Adding Terran Barracks");
		return true;
	}

	//Broodwar->sendText("BuildManager: Unrecognized unit");
	return false;
}

/**
 * Finds suitable position for building. \n
 * Copied and modified from http://www.sscaitournament.com/index.php?action=tutorial
 */ 
TilePosition BuildManager::getBuildPosition(TilePosition aroundTile, UnitType uType){
	TilePosition ret = TilePositions::None;
	int maxDist = 3;
	int stopDist = 40;

	// Refinery, Assimilator, Extractor
	if (uType.isRefinery()) {
			Unit* unit = workerManager->getNearestGeyzer();
			if(unit != NULL) return unit->getTilePosition();
			else {return TilePositions::None; Broodwar->sendText("Problem");}
	}
	
	while ((maxDist < stopDist) && (ret == TilePositions::None)) {
		if(nextPlace){
			for (int i=aroundTile.x()-maxDist; i<=aroundTile.x()+maxDist; i++) {
				for (int j=aroundTile.y()-maxDist; j<=aroundTile.y()+maxDist; j++) {
					if (Broodwar->canBuildHere(NULL, TilePosition(i,j), uType, false)) {
						// units that are blocking the tile

						boolean unitsInWay = false;
						std::set<Unit*>::iterator it;
						std::set<Unit*> myUnits = Broodwar->self()->getUnits();
						
						for (it = myUnits.begin(); it != myUnits.end(); ++it) {
							if ((abs((*it)->getTilePosition().x()-i) < 4) && (abs((*it)->getTilePosition().y()-j) < 4)) unitsInWay = true;
						}
						if (!unitsInWay) {
							nextPlace = !nextPlace;
							return TilePosition(i, j);
						}
						// creep for Zerg
						if (uType.requiresCreep()) {
							boolean creepMissing = false;
							for (int k=i; k<=i+uType.tileWidth(); k++) {
								for (int l=j; l<=j+uType.tileHeight(); l++) {
									if (!Broodwar->hasCreep(k, l)) creepMissing = true;
									break;
								}
							}
							if (creepMissing) continue; 
						}
					}
				}
			}
		
		}
		else{
			for (int i=aroundTile.x()+maxDist; i>=aroundTile.x()-maxDist; i--) {
				for (int j=aroundTile.y()+maxDist; j>=aroundTile.y()-maxDist; j--) {
					if (Broodwar->canBuildHere(NULL, TilePosition(i,j), uType, false)) {
						// units that are blocking the tile

						boolean unitsInWay = false;
						std::set<Unit*>::iterator it;
						std::set<Unit*> myUnits = Broodwar->self()->getUnits();
						
						for (it = myUnits.begin(); it != myUnits.end(); ++it) {
							if ((abs((*it)->getTilePosition().x()-i) < 4) && (abs((*it)->getTilePosition().y()-j) < 4)) unitsInWay = true;
						}
						if (!unitsInWay) {
							nextPlace = !nextPlace;
							return TilePosition(i, j);
						}
						// creep for Zerg
						if (uType.requiresCreep()) {
							boolean creepMissing = false;
							for (int k=i; k<=i+uType.tileWidth(); k++) {
								for (int l=j; l<=j+uType.tileHeight(); l++) {
									if (!Broodwar->hasCreep(k, l)) creepMissing = true;
									break;
								}
							}
							if (creepMissing) continue; 
						}
					}
				}
			}
		}
		maxDist += 2;
	}
	
	if (ret == TilePositions::None) Broodwar->sendText("Unable to find suitable build position for %s", uType.c_str());
	return ret;


}
