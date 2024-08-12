#include "include/MainManager.h"

int main(int argc, char* argv[]) {
    MainManager manager;
    std::cout << "MainManager" << std::endl;
    manager.run(argc, argv);
    std::cout << "out MainManager" << std::endl;
    return 0;
}