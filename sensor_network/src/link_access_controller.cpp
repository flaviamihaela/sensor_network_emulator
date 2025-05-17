#include "link_access_controller.hpp"

// Singleton accessor
LinkAccessController& LinkAccessController::GetInstance() {
    static LinkAccessController lac_instance(Receiver::GetInstance());
    return lac_instance;
}

// Constructor
LinkAccessController::LinkAccessController(Receiver& r): myReceiver(r), numOfAvailableLinks(NUM_OF_LINKS) {
    for (int i = 0; i < NUM_OF_LINKS; ++i) {
        commsLinks.emplace_back(myReceiver, i); // Create Link
        commsLinks[i].attachObserver(this); // Attach this controller as observer 
    }
}

// Request an available communication link
// Block the thread if both links are currently in use
Link& LinkAccessController::requestLink() {
    std::unique_lock<std::mutex> LAC_ul(LAC_mu);

    // Wait until at least one link becomes available
    while (commsLinks[0].isInUse() && commsLinks[1].isInUse()) {
        std::stringstream s6;
        s6 << "Thread " << search() << " is suspended on links" << std::endl;
        printing(s6.str());
        LAC_cv.wait(LAC_ul); // Wait for notification
    }

    // Find and return the first available link
    for (Link& link : commsLinks) {
        if (!link.isInUse()) {
            link.setInUse();
            return link;
        }
    }

    throw std::runtime_error("No available link found, despite passing wait loop.");
}

// Release communication link and mark as idle
void LinkAccessController::releaseLink(Link& releasedLink) {
    std::unique_lock<std::mutex> LAC_ul(LAC_mu);
    releasedLink.setIdle();
}

// Called by a Link to notify idleness
void LinkAccessController::update(Link* link) {
    if (!link->isInUse()) {
    LAC_cv.notify_one(); // Wake up one waiting thread
    }
}
