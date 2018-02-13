
#pragma once

#include <memory>

#include "../net.h"

namespace zia::api {
    struct ImplSocket {
        void sendMessage();

        std::string receiveMessage();
    };
}
