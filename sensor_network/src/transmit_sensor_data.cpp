#include "thread_task.hpp"

void transmitSensorData(LinkAccessController& lac, int idx, const SensorData& t_s_data, const SensorData& p_s_data, const SensorData& c_s_data) {
    // Request comms link
    Link* link = &lac.requestLink();

    std::stringstream s4;
    s4 << "Thread " << idx << " successfully acquired link " << link->getLinkId() << std::endl;
    printing(s4.str());

    // Transmit collected data mix to Receiver
    link->writeToDataLink(t_s_data);
    link->writeToDataLink(p_s_data);
    link->writeToDataLink(c_s_data);

    // Release comms link
    lac.releaseLink(*link);

    std::stringstream s5;
    s5 << "Thread " << idx << " successfully released link " << link->getLinkId() << std::endl;
    printing(s5.str());

    std::this_thread::sleep_for(std::chrono::milliseconds((dis(gen) % 10) + 1));
}
