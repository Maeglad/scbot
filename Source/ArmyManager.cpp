#include "ArmyManager.h"
using namespace BWAPI;

ArmyManager::ArmyManager(){
	academy = NULL;
	medicRatio = 4;
	marines.clear();
	medics.clear();
	buildings.clear();
	SquadManager* baseSquad = new SquadManager;
	squads.clear();
	squads.insert(baseSquad);
}

ArmyManager::~ArmyManager(){
}

/**
 * Called from buildManager, should determine what enemy is creating and decide against it but 
 * right now it just creates marines and medics in ratio 4:1
 */
BWAPI::UnitType ArmyManager::getArmyOrder(){
	if((academy == NULL)||(academy->isBeingConstructed())){
		return UnitTypes::Terran_Marine;
	}
	else{
		if(((marines.size()+1) / (medics.size()+1)) > medicRatio){
			Broodwar->sendText("Medic");
			return UnitTypes::Terran_Medic;
		}
		else{
			Broodwar->sendText("Marine");
			return UnitTypes::Terran_Marine;
		}
	}
}
/**
 * Non-functional
 */
void ArmyManager::drawDebug(){
}

/**
 * Called from BotModule on onFrame(). \n
 * calls: \n
 *		update - delete dead units from list\n
 *		if building is under attack calls all idle == 0, squads to defend its
 */
void ArmyManager::onFrame(){
	//drawDebug();
	update();
	std::set<SquadManager*>::iterator it;
	for(it = squads.begin(); it != squads.end(); ++it){
		(*it)->onFrame();
	}
	std::set<Unit*>::iterator iter;
	for(iter = buildings.begin(); iter != buildings.end(); ++iter){
		if((*iter)->isUnderAttack()){
			for(it = squads.begin(); it!= squads.end(); ++it){
				if((*it)->state == 0){
					(*it)->setTarget((*iter)->getPosition());
				}
			}
		}
	}
}

void ArmyManager::update(){
	std::set<BWAPI::Unit*>::iterator it;
	for(it = marines.begin(); it != marines.end(); ++it){
		if((*it)->getHitPoints() <= 0){
			marines.erase(it);// invalidation;
			return;
		}
	}
	for(it = medics.begin(); it != medics.end(); ++it){
		if((*it)->getHitPoints() <= 0){
			medics.erase(it);// invalidation;
			return;
		}
	}
}
/**
 * Should register enemies in future right now just adds my military units
 */
bool ArmyManager::add(BWAPI::Unit* unit){
	if(Broodwar->self()->isEnemy(unit->getPlayer())){// momentalne nepriatelov ignorujem
		return false;
	}

	

	UnitType uType = unit->getType();

	

	if(uType == UnitTypes::Terran_Marine){
		marines.insert(unit);
		(*squads.begin())->add(unit);
		return true;
	}

	if(uType == UnitTypes::Terran_Medic){
		medics.insert(unit);
		(*squads.begin())->add(unit);
		return true;
	}

	if(uType == UnitTypes::Terran_Academy){
		academy = unit;
	}

	if(uType.isBuilding()){
		buildings.insert(unit);
		return true;
	}

	return false;
}