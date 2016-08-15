/**
 * Class containging information shared among all classes
 */ 
#pragma once
#include <BWAPI.h>

struct supplyStruct{
		int total;
		int used;
	};


namespace BWColor
{
	const int RED = 111;
	const int BLUE = 165;
	const int TEAL = 159;
	const int PURPLE = 164;
	const int ORANGE = 179;
	const int BROWN = 19;
	const int WHITE = 255;
	const int YELLOW = 135;
	const int GREEN = 117;
	const int CYAN = 128;
	const int BLACK = 0;
	const int GREY = 74;
};

class DataManager;
class WorkerManager;
class ArmyManager;
class BuildManager;

// Initialized in Botmodule::onStart();
extern DataManager* dataManager;
extern WorkerManager * workerManager;
extern ArmyManager* armyManager;
extern BuildManager* buildManager;