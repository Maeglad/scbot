#include "BotModule.h"
using namespace BWAPI;


/**
 * Destructor \n 
 * TODO
 */
BotModule::~BotModule(){

}
/**
 * Called when bot send text
 */
void BotModule::onSendText(std::string text)
{
}

/**
 * Called when one of players send text
 */
void BotModule::onReceiveText(BWAPI::Player* player, std::string text)
{
}

/**
 * Called on start of game. \n
 * Contains manager declaration and few debug values
 */
void BotModule::onStart()
{
	// analyze map by BWTA to access cool features
	BWTA::readMap();
	BWTA::analyze();
	// end of analysis
		
	//debug info
	Broodwar->sendText("IT IS ALIVE.");
	std::string mapName = Broodwar->mapName();
	int startLocationsCount = Broodwar->getStartLocations().size();
	Broodwar->printf("The map is %s, a %d player map.", mapName.c_str(), startLocationsCount);
	//debug info

	//cheat flags
	Broodwar->enableFlag(BWAPI::Flag::UserInput);
	Broodwar->sendText("whats mine is mine");
	Broodwar->sendText("whats mine is mine");
	Broodwar->sendText("whats mine is mine");
	Broodwar->sendText("whats mine is mine");
	//Broodwar->sendText("operation cwal");
	Broodwar->setLocalSpeed(5);
	//cheat flags

	// managers

	dataManager = new DataManager();// = new DataManager();
	workerManager = new WorkerManager();
	buildManager = new BuildManager();
	armyManager = new ArmyManager();

	// managers end;
}

/**
 * Called on every frame of game. \n
 * Calls onFrame method of managers nad drawDebug from dataManager.
 */ 
void BotModule::onFrame()
{
	workerManager->onFrame();
	buildManager->onFrame();
	dataManager->drawDebug();
	armyManager->onFrame();
}

/**
 * Called when unit is discovered
 */
void BotModule::onUnitDiscover(BWAPI::Unit* unit)
{
}

void BotModule::onEnd(bool isWinner)
{
}

void BotModule::onSaveGame(std::string gameName)
{
}

void BotModule::onPlayerLeft(BWAPI::Player* player)
{
}


/**
 * Called when new unit is created
 */
void BotModule::onUnitCreate(BWAPI::Unit* unit)
{
}

/**
 * Called when unit is destroyed
 */ 
void BotModule::onUnitDestroy(BWAPI::Unit* unit)
{
//TODO
// add armyManager asses
// add *manager remove();
}


void BotModule::onUnitEvade(BWAPI::Unit* unit)
{
}
/**
 * Called when unit becomes visible. \n
 * Calls add() method of managers. \n
 * Used in handling all bots units
 */
void BotModule::onUnitShow(BWAPI::Unit* unit)
{
	workerManager->add(unit);
	buildManager->add(unit);
	armyManager->add(unit);

}

void BotModule::onUnitHide(BWAPI::Unit* unit)
{
}
/**
 * Called when unit morphs. \n
 * Used to handle refinery adding.
 */
void BotModule::onUnitMorph(BWAPI::Unit* unit)
{
	// snad sa mi to neskor podari presunut ale temp riesenie je proste sledovat morf geyziru
	workerManager->add(unit);
	buildManager->add(unit); 
}

void BotModule::onUnitComplete(BWAPI::Unit *unit)
{
}

void BotModule::onUnitRenegade(BWAPI::Unit* unit)
{
}

void BotModule::onNukeDetect(BWAPI::Position target)
{
}

