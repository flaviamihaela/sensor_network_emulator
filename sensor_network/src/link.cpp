#include "link.hpp"
#include "link_access_controller.hpp"

// Constructor
Link::Link(Receiver& r, int linkNum): inUse(false), myReceiver(r), linkId(linkNum), observer(nullptr) {}

// Attach LAC as observer for a link
void Link::attachObserver(LinkAccessController* lac) {
    observer = lac;
}

// Notify LAC that link us idle
void Link::notifyObserver() {
    if (observer) {
        observer->update(this);
    }
}

// Return true if link is in use
bool Link::isInUse() const {
    return inUse;
}

// Mark link as in use 
void Link::setInUse() {
    inUse = true;
}

// Mark link as idle
void Link::setIdle() {
    inUse = false;
    notifyObserver();
}

// Send sensor data through link to Receiver
void Link::writeToDataLink(SensorData sd) {
    myReceiver.receiveData(sd);
}

// Return unique identifier for link
int Link::getLinkId() const {
    return linkId;
}
