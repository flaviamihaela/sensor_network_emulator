#include "thread_task.hpp"

void acquireSensorData(BC& theBC, int idx, SensorData& t_s_data, SensorData& p_s_data, SensorData& c_s_data) {
    int sel; // Sensor random selection

    double val; // Sensor value read
    std::string typ; // Sensor type

    for (int i = 0; i < NUM_OF_SAMPLES; i++) {
        // Other threads cannot use BC 
        theBC.requestBC();

        // Random sensor selection
        sel = dis(gen) % 3;

        // Read data
        val = theBC.getSensorValue(sel);
        typ = theBC.getSensorType(sel);

        std::stringstream s2;
        s2 << " sample value from thread " << idx << " of type " << typ << " is equal to " << val << std::endl;
        printing(s2.str());

        // Accumulate data for thread
        if (typ == t_s) {
            counter_ts++;
            t_s_data.addData(val);
        } else if (typ == p_s) {
            counter_ps++;
            p_s_data.addData(val);
        } else if (typ == c_s) {
            counter_cs++;
            c_s_data.addData(val);
        }

        theBC.releaseBC();
        // Let other threads grab BC
        std::this_thread::sleep_for(std::chrono::milliseconds((dis(gen) % 10) + 1));
    }
}

