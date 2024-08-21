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
void MySimulator::prepareSimulationEnvironment(std::shared_ptr<House> housePtr, std::string houseFilePath, std::string algoName) {
	setHouse(housePtr);
	setSensors();
	om = OutputManager(houseFilePath, algoName);
}

void MySimulator::setHouse(std::shared_ptr<House> housePtr) {
	house = housePtr;
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
void MySimulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm> algo) {
	myAlgo = std::move(algo);
	myAlgo->setMaxSteps(maxSteps);
	myAlgo->setWallsSensor(wallsSensor);
	myAlgo->setDirtSensor(dirtSensor);
	myAlgo->setBatteryMeter(batteryMeter);
	Logger::getInstance().log("Done setting algorithm and its sensors.\n", LogLevels::FILE);
	//getTimeout();
}

/*
	Run robot- make steps according algorithm decision as long as:"continueWorking"
*/
void MySimulator::run() {
	std::cout << "in run " << std::endl;
	while (numberOfStepsMade <= house->getMaxSteps()) {

		std::cout <<"myAlgo -> nextStep() " << std::endl;
		if (myAlgo) {
			std::cout <<"myAlgo" << std::endl;
		}
		else {
			std::cout <<"!myAlgo" << std::endl;
		}

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
	score = calculateScore();

	std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";
	
}

/*
	Write output using outputManager, return score calculated there
*/
void MySimulator::setOutput() {
	om.writeOutput(steps, numberOfStepsMade, house->getAmountOfDirt(), status, house->inDockingStation(), score);
	om.displaySim();
    std::cout.flush(); 
}

int MySimulator::getScore(){
	return score;
}

int MySimulator::calculateScore() {
	int amountOfDirtLeft = house->getAmountOfDirt();
	bool inDocking = house->inDockingStation();
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
	std::cout << "calculateScore" << std::endl;
	std::cout << "amountOfDirtLeft " << amountOfDirtLeft << std::endl;
	std::cout << "inDocking " << inDocking << std::endl;
	std::cout << "maxSteps " << maxSteps << std::endl;
	std::cout << "numberOfStepsMade " << numberOfStepsMade << std::endl;
    return numberOfStepsMade + amountOfDirtLeft * 300 + (inDocking ? 0 : 1000);
}