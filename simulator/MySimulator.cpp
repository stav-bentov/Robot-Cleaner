#include "include/my_simulator.h"

MySimulator::MySimulator(std::atomic<bool>& stopFlag_)
    : myAlgo(nullptr),
      house(nullptr),
	  om("", ""),
	  status("WORKING"),
	  numberOfStepsMade(0),
	  stopFlag(stopFlag_) {}

/*
	Building House object from house file.
	Builds corresponding sensors.
	Set outputManager (input and ouput file name and house name)
*/
void MySimulator::prepareSimulationEnvironment(std::string houseFilePath, std::string algoName) {
	
    std::cout << "MySimulator::prepareSimulationEnvironment" << std::endl;
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
    std::cout << "MySimulator::setHouse" << std::endl;
	house = std::make_shared<House>(houseFilePath);
	maxSteps = house->getMaxSteps();
	dockingStationLocation = house->getDockingStationLocation();
	currentLocation = dockingStationLocation;
	Logger::getInstance().log("Done loading the house.\n", LogLevels::FILE);
}

void MySimulator::setSensors() {
    std::cout << "MySimulator::setSensors" << std::endl;
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
	myAlgo = &algo;
	if (myAlgo) {
    	std::cout << "myAlgo not empty" << std::endl;
	}
	else
	{
    	std::cout << "myAlgo empty" << std::endl;
	}
    std::cout << "MySimulator::setAlgorithm" << std::endl;
	std::cout << "algo.setMaxSteps(maxSteps);" << std::endl;
	myAlgo->setMaxSteps(maxSteps);
	std::cout << "algo.setWallsSensor(wallsSensor);" << std::endl;
	myAlgo->setWallsSensor(wallsSensor);
	std::cout << "algo.setDirtSensor(dirtSensor);" << std::endl;
	algo.setDirtSensor(dirtSensor);
	std::cout << "algo.setBatteryMeter(batteryMeter);" << std::endl;
	myAlgo->setBatteryMeter(batteryMeter);
	std::cout << "myAlgo = &algo;;" << std::endl;
	Logger::getInstance().log("Done setting algorithm and its sensors.\n", LogLevels::FILE);
}

/*
	Run robot- make steps according algorithm decision as long as:"continueWorking"
*/
void MySimulator::run() {
	while (numberOfStepsMade <= house->getMaxSteps()) {
		if (stopFlag.load()) {
			std::cout << "GOT stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag " << std::endl;
			break;
		}
		Step currentStep = myAlgo -> nextStep();
		steps.push_back(currentStep);
		Common::logStep(currentStep);

		if (currentStep == Step::Finish) {
			status = "FINISHED";
			break;
		}
		
		//std::cout << "house->makeStep(currentStep);" << std::endl;
		house->makeStep(currentStep);
		numberOfStepsMade++;
		
	}
	score = calculateScore(numberOfStepsMade, status, house->getAmountOfDirt(), house->inDockingStation(), maxSteps);

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
	om.writeOutput(steps, numberOfStepsMade, house->getAmountOfDirt(), status, house->inDockingStation(), score);
	om.displaySim();
    std::cout.flush(); 
}

int MySimulator::getScore() const{
	return score;
}

int MySimulator::calculateScore(std::size_t numSteps, std::string status, int amountOfDirtLeft, bool inDocking, std::size_t maxSteps) {
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