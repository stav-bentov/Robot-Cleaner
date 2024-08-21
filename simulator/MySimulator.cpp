#include "include/my_simulator.h"

MySimulator::MySimulator()
    : myAlgo(nullptr),
      house(nullptr),
	  om("", ""),
	  status("WORKING"),
	  numberOfStepsMade(0) {}

void MySimulator::getTimeout() {
	std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

	// TODO: maybe add handle error?
    SimConfigurationManager simConfigManager;
    int timePerStep = simConfigManager.getTimePerStep();
	std::cout << thread<<" maxSteps " << maxSteps<<std::endl;
	std::cout << thread <<" timePerStep " << timePerStep<<std::endl;
	timeoutDuration = std::chrono::milliseconds(timePerStep * maxSteps);
	auto durationInMilliseconds = timeoutDuration.count(); // Get the number of milliseconds

    // Print the duration
    std::cout << thread <<" timeoutDuration: " << durationInMilliseconds << " milliseconds" << std::endl;

}

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
void MySimulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm> algo) {
	myAlgo = std::move(algo);
	myAlgo->setMaxSteps(maxSteps);
	myAlgo->setWallsSensor(wallsSensor);
	myAlgo->setDirtSensor(dirtSensor);
	myAlgo->setBatteryMeter(batteryMeter);
	Logger::getInstance().log("Done setting algorithm and its sensors.\n", LogLevels::FILE);
	getTimeout();
}

/*
	Run robot- make steps according algorithm decision as long as:"continueWorking"
*/
void MySimulator::run() {
	bool stopFlag = false;
	std::cout << "in run " << std::endl;
    auto startTime = std::chrono::steady_clock::now();
	while (numberOfStepsMade <= house->getMaxSteps()) {
		auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = currentTime - startTime;

        if (elapsedTime > timeoutDuration) {
			stopFlag = true;
			break;
        }

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
	score = calculateScore(numberOfStepsMade, status, house->getAmountOfDirt(), house->inDockingStation(), maxSteps);

	if (((batteryMeter.getBatteryState() == 0 && !house->inDockingStation()))){
		status = "DEAD";
	}

	if ((status == "FINISHED" && !house->inDockingStation()))
	{
		//Logger::getInstance().getLogger()->info("Error: algorithm returned FINISHED and not in docking station");
	}
	std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";
	if (stopFlag) {
		std::cout <<thread<< "GOT stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag " << std::endl;
	}
	else
	{
		std::cout <<thread<< "NOT GOT stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag  stopFlag " << std::endl;
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