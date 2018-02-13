
#pragma once

#include <memory>

#include "../net.h"

namespace zia::apipp {

    class NetSocket : zia::api::ImplSocket {
        void sendMessage();
        std::string receiveMessage();
    };

}
