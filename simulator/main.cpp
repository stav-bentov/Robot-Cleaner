#include "include/MainManager.h"

int main(int argc, char* argv[]) {
    try {
        MainManager manager;
        manager.run(argc, argv);
        std::cout << "end main" <<std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}