#include "include/my_simulator.h"

MySimulator::MySimulator()
    : myAlgo(nullptr),
      house(nullptr),
	  status("WORKING"),
	  numberOfStepsMade(0) {}

/*
	Building House object from house file.
	Builds corresponding sensors.
	Set outputManager (input and ouput file name and house name)
*/
void MySimulator::prepareSimulationEnvironment(std::shared_ptr<House> housePtr, std::string& housePath_, std::string& algoName_) {
	housePath = housePath_;
	algoName = algoName_;
	setHouse(housePtr);
	setSensors();
}

void MySimulator::setHouse(std::shared_ptr<House> housePtr) {
	house = housePtr;
	maxSteps = house->getMaxSteps();
	dockingStationLocation = house->getDockingStationLocation();
	currentLocation = dockingStationLocation;
    score = maxSteps * 2 + house->getAmountOfDirt() * 300 + 2000;
} 

void MySimulator::setSensors() {
	// Create sensors
	wallsSensor = RobotWallsSensor(house);
	dirtSensor = RobotDirtSensor(house);
	batteryMeter = RobotBatteryMeter(house);
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
}

/*
	Run robot- make steps according algorithm decision as long as:"continueWorking"
*/
void MySimulator::run() {
	while (numberOfStepsMade <= house->getMaxSteps()) {
		Step currentStep = myAlgo -> nextStep();
		steps.push_back(currentStep);
		//Common::logStep(currentStep);

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
	
}

/*
	Write output using outputManager, return score calculated there
*/
void MySimulator::setOutput() {
	OutputManager om(housePath, algoName);  
	om.writeOutput(steps, numberOfStepsMade, house->getAmountOfDirt(), status, house->inDockingStation(), score);
	om.displaySim();
    std::cout.flush(); 
}

int MySimulator::getScore(){
	return score;
}

void MySimulator::calculateScore() {
	int amountOfDirtLeft = house->getAmountOfDirt();
	bool inDocking = house->inDockingStation();
    if (status == "DEAD")
    {
        score = maxSteps + amountOfDirtLeft * 300 + 2000;
    }
    else if (status == "FINISHED" && !inDocking)
    {
      //  Logger::getInstance().log("ERROR, status == FINISHED && !inDocking.\n", LogLevels::FILE);
        score = maxSteps + amountOfDirtLeft * 300 + 3000;
    }
    // else
    score = numberOfStepsMade + amountOfDirtLeft * 300 + (inDocking ? 0 : 1000);
}

int MySimulator::getnumberOfStepsMade() {
	return numberOfStepsMade;
}