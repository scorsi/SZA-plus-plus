#include <iostream>
#include "api/conf.h"
#include "api/http.h"
#include "api/module.h"
#include "api/net.h"
#include "api/pp/module.hpp"

class Test : public zia::apipp::Module {
private:
    zia::api::Conf conf;

public:
    ~Test() override = default;

    bool perform() override {
        this->response->setStatus(200, "OK")
                ->addHeader("Set-Cookie", "test")
                ->addHeader("Set-Cookie", "toto")
                ->body = "test";
    }
};

int main() {
    Test module = Test();
    zia::api::HttpDuplex duplex;

    module.exec(duplex);

    for (const auto &item: duplex.resp.body) {
        std::cout << static_cast<char>(item);
    }
    std::cout << std::endl;

    for (const auto &item: duplex.resp.headers) {
        std::cout << item.first << "=" << item.second << std::endl;
    }
}
