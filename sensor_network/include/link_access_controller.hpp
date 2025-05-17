#pragma once

#include "link.hpp"
#include "receiver.hpp"
#include "utils.hpp"
#include "globals.hpp"
#include <vector>
#include <mutex>
#include <condition_variable>

class LinkAccessController {
    public:
        static LinkAccessController& GetInstance();

        Link& requestLink();
        void releaseLink(Link& releasedLink);
        void update(Link* link);

    private:
        LinkAccessController(Receiver& r);

        // Disable copy & move
        LinkAccessController(const LinkAccessController&) = delete;
        LinkAccessController& operator=(const LinkAccessController&) = delete;

        Receiver& myReceiver;
        int numOfAvailableLinks;
        std::vector<Link> commsLinks;
        std::mutex LAC_mu;
        std::condition_variable LAC_cv;
};
