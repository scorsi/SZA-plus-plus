//
// Created by scorsi on 23/12/17.
//

#include <iostream>
#include "api/pp/conf.hpp"
#include "api/pp/net.hpp"
#include "api/pp/http.hpp"
#include "api/pp/module.hpp"

// SZA++ Module
class Test1 : public zia::apipp::Module {
public:
    ~Test1() override = default;

    bool perform() override {
        this->response->useStandardData();

        this->response
                ->setStatus(42, "42 power")
                ->addHeader("Set-Cookie", "test")
                ->setStandardData("test");
    }
};

// Basic SZA Module
class Test2 : public zia::api::Module {
private:
    zia::api::Conf conf;

public:
    ~Test2() override = default;

    bool config(const zia::api::Conf &conf) override {
        this->conf = conf;
        return true;
    }

    bool exec(zia::api::HttpDuplex &duplex) override {
        duplex.resp.status = 200;
        duplex.resp.reason = "OK";

        duplex.resp.headers["Set-Cookie"] += ",hey";

        // Append equivalent
        for (const auto &item: "biloute") {
            duplex.resp.body.push_back(static_cast<std::byte>(item));
        }
        duplex.resp.body.pop_back(); // Remove useless \0 caused by the foreach loop

        return true;
    }

};

// SZA++ Module
class Test3 : public zia::apipp::Module {
public:
    ~Test3() override = default;

    bool perform() override {
        // Set to the response that we use Standard Data
        this->response->useStandardData();

        // Initialize the response
        this->response
                ->addHeader("Set-Cookie", "toto")
                ->appendStandardData("toto");
    }
};

void test2() {
    // We use "zia::api::Module" here
    // We shall use raw pointer because we must use dynamic_cast
    std::vector<zia::api::Module *> modules;
    modules.push_back(new Test1());
    modules.push_back(new Test2());
    modules.push_back(new Test3());

    zia::apipp::NetPtr net = std::make_shared<zia::apipp::Net>();

    // We create a false request
    zia::apipp::RequestPtr request = std::make_shared<zia::apipp::Request>(zia::api::http::Version::http_1_1,
                                                                           zia::api::http::Method::get,
                                                                           "/test");

    // We create the response based on the request
    zia::apipp::ResponsePtr response = std::make_shared<zia::apipp::Response>(*request);

    for (const auto &mod: modules) {
        {
            // SZA/SZA++ Compliant code
            if (auto *modpp = dynamic_cast<zia::apipp::Module *>(mod)) {
                std::cout << "Executing a sza++ module" << std::endl;

                modpp->smartExec(request, response, net);
            } else {
                std::cout << "Executing a basic module" << std::endl;

                zia::api::HttpDuplex duplex = zia::apipp::createBasicHttpDuplex(request, response, net);
                mod->exec(duplex);
                response = zia::apipp::Response::fromBasicHttpDuplex(duplex);
                request = zia::apipp::Request::fromBasicHttpDuplex(duplex);
            }
        }
        std::cout << "Response:" << response->body << std::endl;
        std::cout << response->statusCode << ": " << response->statusReason << std::endl;
        for (const auto &item: response->headers) {
            for (const auto &value: item.second) {
                std::cout << item.first << ": " << value << std::endl;
            }
        }
        std::cout << std::endl << std::endl;
    }
}