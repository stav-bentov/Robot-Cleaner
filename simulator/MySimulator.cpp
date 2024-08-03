#include "../include/my_simulator.h"

MySimulator::MySimulator()
    : myAlgo(nullptr) {}

/*
	Building House object from house file.
	Builds corresponding sensors.
*/
void MySimulator::readHouseFile(std::string& houseFileName) {
	house = std::make_shared<House>(houseFileName);
	maxSteps = house->getMaxSteps();
	dockingStationLocation = house->getDockingStationLocation();
	currentLocation = dockingStationLocation;

	Logger::getInstance().getLogger()->info("Done loading the house.");
	
	// Create sensors
	wallsSensor = RobotWallsSensor(house);
	dirtSensor = RobotDirtSensor(house);
	batteryMeter = RobotBatteryMeter();
	Logger::getInstance().getLogger()->info("Done setting sensors.");

	om.setInputName(houseFileName);
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
	Logger::getInstance().getLogger()->info("Done setting sensors and algorithms");
}

/*
	Run robot- make steps according algorithm decision as long as:"continueWorking"
*/
void MySimulator::run() {
	std::vector<Step> steps;
	std::string status = "WORKING";

	while (true) {
		Step currentStep = myAlgo -> nextStep();
		steps.push_back(currentStep);
		logStep(currentStep);

		if (currentStep == Step::Finish) {
			status = "FINISHED";
			break;
		}
		
		house->makeStep(currentStep);
	}

	if (((batteryMeter.getBatteryState() == 0 && !house->inDockingStation()))){
		status = "DEAD";
	}

	if ((status == "FINISHED" && !house->inDockingStation()))
	{
		Logger::getInstance().getLogger()->info("Error: algorithm returned FINISHED and not in docking station");
	}

	Logger::getInstance().getLogger()->info("Done setting sensors.");
	Logger::getInstance().getLogger()->info("The End: " + house->inDockingStation());
	om.writeOutput(steps, house->getTotalAmountOfSteps(), house->getAmountOfDirt(), status);
	om.displaySim();
    std::cout.flush(); 
}

void MySimulator::logStep(Step s) {
	switch (s)
	{
		case Step::Stay:
			Logger::getInstance().getLogger()->info("Simulator got Step::Stay");
			break;
		case Step::East:
			Logger::getInstance().getLogger()->info("Simulator got Step::East");
			break;
		case Step::West:
			Logger::getInstance().getLogger()->info("Simulator got Step::West");
			break;
		case Step::South:
			Logger::getInstance().getLogger()->info("Simulator got Step::South");
			break;
		case Step::North:
			Logger::getInstance().getLogger()->info("Simulator got Step::North");
			break;
		case Step::Finish:
			Logger::getInstance().getLogger()->info("Simulator got Step::Finish");
			break;
	}
}