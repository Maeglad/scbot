/**
 * Main class. \n
 * handles
 *  - start of bot 
 *  - calls onFrame methods of managers
 *  - calls add methods of managers
 */
#pragma once

#include <windows.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "Shared.h"
#include "DataManager.h"
#include "WorkerManager.h"
#include "BuildManager.h"
#include "ArmyManager.h"


class BotModule : public BWAPI::AIModule
{
public:
	~BotModule();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player* player, std::string text);
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onSaveGame(std::string gameName);
	virtual void onPlayerLeft(BWAPI::Player* player);

	virtual void onFrame();

//	virtual bool isAlive(BWAPI::Unit* unit);

	virtual void onUnitCreate(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);

	virtual void onUnitDiscover(BWAPI::Unit* unit);
	virtual void onUnitEvade(BWAPI::Unit* unit);

	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);

	virtual void onUnitMorph(BWAPI::Unit* unit);

	virtual void onUnitComplete(BWAPI::Unit *unit);

	virtual void onUnitRenegade(BWAPI::Unit* unit);

	virtual void onNukeDetect(BWAPI::Position target);

//	void drawDebug();
//	void drawDebug2();

	//void drawText(std::string text);
};

