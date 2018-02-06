
#pragma once

#include <memory>

#include "../net.h"


namespace zia::apipp {

    class Net {
    public:
        static std::shared_ptr<Net> fromBasicNetInfo(zia::api::NetInfo &netInfo) {
            return std::make_shared<Net>();
        }

        zia::api::NetInfo toBasicNetInfo() const {
            return zia::api::NetInfo();
        }
    };

    typedef std::shared_ptr<Net> NetPtr;

}
