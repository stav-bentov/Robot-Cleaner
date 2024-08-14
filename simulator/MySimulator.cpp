#include "include/my_simulator.h"

MySimulator::MySimulator()
    : myAlgo(nullptr),
      house(nullptr),
	  om("", ""),
	  status("WORKING"),
	  numberOfStepsMade(0) {}

/*
	Building House object from house file.
	Builds corresponding sensors.
	Set outputManager (input and ouput file name and house name)
*/
void MySimulator::prepareSimulationEnvironment(std::string houseFilePath, std::string algoName) {
	try {
		setHouse(houseFilePath);
		setSensors();
		om = OutputManager(houseFilePath, algoName);
    }
    catch (const std::exception& e) {
        ErrorManager::checkForError(true, "Error: in house constructor");
    }
}

void MySimulator::setHouse(std::string houseFilePath) {
	house = std::make_shared<House>(houseFilePath);
	maxSteps = house->getMaxSteps();
	dockingStationLocation = house->getDockingStationLocation();
	currentLocation = dockingStationLocation;
	Logger::getInstance().log("Done loading the house.\n", LogLevels::FILE);
}

void MySimulator::setSensors() {
	// Create sensors
	wallsSensor = RobotWallsSensor(house);
	dirtSensor = RobotDirtSensor(house);
	batteryMeter = RobotBatteryMeter(house);
	Logger::getInstance().log("Done setting sensors.\n", LogLevels::FILE);
}

/*
	Sets sensors to algorithm.
*/
void MySimulator::setAlgorithm(AbstractAlgorithm& algo) {
	algo.setMaxSteps(maxSteps);
	algo.setWallsSensor(wallsSensor);
	algo.setDirtSensor(dirtSensor);
	algo.setBatteryMeter(batteryMeter);
	myAlgo = &algo;
	Logger::getInstance().log("Done setting algorithm and its sensors.\n", LogLevels::FILE);
}

/*
	Run robot- make steps according algorithm decision as long as:"continueWorking"
*/
void MySimulator::run() {
	while (numberOfStepsMade <= house->getMaxSteps()) {
		Step currentStep = myAlgo -> nextStep();
		steps.push_back(currentStep);
		Common::logStep(currentStep);

		if (currentStep == Step::Finish) {
			status = "FINISHED";
			break;
		}
		
		house->makeStep(currentStep);
		numberOfStepsMade++;
	}

	if (((batteryMeter.getBatteryState() == 0 && !house->inDockingStation()))){
		status = "DEAD";
	}

	if ((status == "FINISHED" && !house->inDockingStation()))
	{
		//Logger::getInstance().getLogger()->info("Error: algorithm returned FINISHED and not in docking station");
	}
}

/*
	Write output using outputManager, return score calculated there
*/
void MySimulator::setOutput() {
	int score = getScore(numberOfStepsMade, status, house->getAmountOfDirt(), house->inDockingStation(), maxSteps);
	om.writeOutput(steps, numberOfStepsMade, house->getAmountOfDirt(), status, house->inDockingStation(), score);
	om.displaySim();
    std::cout.flush(); 
}


int MySimulator::getScore(std::size_t numSteps, std::string status, int amountOfDirtLeft, bool inDocking, std::size_t maxSteps) {
    if (status == "DEAD")
    {
        return maxSteps + amountOfDirtLeft * 300 + 2000;
    }
    else if (status == "FINISHED" && !inDocking)
    {
        Logger::getInstance().log("ERROR, status == FINISHED && !inDocking.\n", LogLevels::FILE);
        return maxSteps + amountOfDirtLeft * 300 + 3000;
    }
    // else
    return numSteps + amountOfDirtLeft * 300 + (inDocking ? 0 : 1000);
}