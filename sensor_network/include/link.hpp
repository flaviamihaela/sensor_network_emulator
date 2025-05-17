#pragma once

#include "sensor_data.hpp"
#include "receiver.hpp"

class LinkAccessController;

class Link {
    public:
        Link(Receiver& r, int linkNum);

        void attachObserver(LinkAccessController* lac);
        void notifyObserver();

        bool isInUse() const;
        void setInUse();
        void setIdle();

        void writeToDataLink(SensorData sd);
        int getLinkId() const;

    private:
        bool inUse;
        Receiver& myReceiver;
        int linkId;
        LinkAccessController* observer;
};
