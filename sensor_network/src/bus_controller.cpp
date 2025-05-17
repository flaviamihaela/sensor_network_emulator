#include "bus_controller.hpp"
#include "utils.hpp"

// Singleton accessor
BC& BC::GetInstance(std::vector<Sensor*>& sensors) {
    static BC bc_instance(sensors);
    return bc_instance;
}

// Constructor
BC::BC(std::vector<Sensor*>& sensors) : theSensors(sensors) {}

// Request exclusive access to the bus controller
void BC::requestBC() {
    std::unique_lock<std::mutex> ul(BC_mu);
    while (lock) {
        std::stringstream s;
        s << "Bus Controller is locked so thread " << search()<< " is about to suspend..." << std::endl;
        printing(s.str());
        vc.wait(ul);  // Wait until the BC becomes available
    }

    lock = true;
    std::stringstream s1;
    s1 << "Bus Controller locked by thread " << search() << std::endl;
    printing(s1.str());
}

// Read sensor value by index
double BC::getSensorValue(int selector) {
    return theSensors[selector]->getValue();
}

// Read sensor type by index
std::string BC::getSensorType(int selector) {
    return theSensors[selector]->getType();
}

// Release the bus controller lock
void BC::releaseBC() {
    std::unique_lock<std::mutex> ul(BC_mu);
    lock = false;
    vc.notify_one();

    std::stringstream s3;
    s3 << "Bus Controller UNlocked by thread " << search() << std::endl;
    printing(s3.str());
}
