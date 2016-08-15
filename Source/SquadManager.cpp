#include "SquadManager.h"
using namespace BWAPI;

SquadManager::SquadManager(){
	units.clear();
	target = dataManager->getBasePosition();
	radius = 500;
}

SquadManager::~SquadManager(){
	
}
/**
 * I was thinking about using this to split squad into smaller ones, but 
 * I cant think of situation that would realy require it, becouse you usually want to 
 * keep your army as one big group\n
 * note to self: mb flying squad?
 */
SquadManager SquadManager::split(){

}

bool SquadManager::add(BWAPI::Unit* unit){
	units.insert(unit);
	return true;
}
/**
 * Counterpiece to split, also not implemented since there will be no spliting
 */
bool SquadManager::merge(SquadManager* squad){

}

std::set<BWAPI::Unit*> SquadManager::getUnits(){
	return this->units;
}
/**
 * scans area around target in radius, if enemy units are discovered, this squad will attack it
 */
void SquadManager::onFrame(){
	if(state == 0)update();
	std::set<Unit*> scan = Broodwar->getUnitsInRadius(target, radius);
	std::set<Unit*>::iterator it;
	for(it = scan.begin(); it != scan.end(); it++){
		if(Broodwar->self()->isEnemy((*it)->getPlayer())){
			this->attack((*it));
			return;
		}
	}
	state = 0;
}
/**
 * extracts enemy unit position and sends all squad members that are not currently fighting to unit location
 */
void SquadManager::attack(BWAPI::Unit* unit){
	state = 1;
	std::set<Unit*>::iterator it;
	Position pos = unit->getPosition();
	for(it = units.begin(); it != units.end(); ++it){
		if((*it)->isAttacking() == false)
			(*it)->attack(pos);	
	}
}

/**
 * deletes dead units from list and if any of units are too far from their target they are sent back
 */
void SquadManager::update(){
	std::set<Unit*>::iterator it;
	for(it = units.begin(); it != units.end(); it++){
		if((*it)->getHitPoints() <= 0){
			units.erase(it);
			return;
		}
		if((*it) != NULL){
			if(dataManager->getBasePosition().getDistance((*it)->getPosition())> radius)
				(*it)->attack(target);
		}
	}
}

void SquadManager::setTarget(BWAPI::Position pos){
	this->target = pos;
}

void SquadManager::setRadius(int radius){
	this->radius = radius;
}