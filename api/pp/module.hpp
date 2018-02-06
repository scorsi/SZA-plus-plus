
#pragma once

#include "../module.h"

#include "conf.hpp"
#include "http.hpp"
#include "net.hpp"

namespace zia::apipp {

    class Module : public zia::api::Module {
    protected:
        Conf conf{};
        ResponsePtr response{};
        RequestPtr request{};
        NetPtr net{};

    public:
        ~Module() override = default;

        bool config(const zia::api::Conf &conf) override {
            this->conf = Conf::fromBasicConfig(conf);
            return true;
        }

        void reset() {
            // We must be sure that we correctly releasing pointers
            this->request.reset();
            this->response.reset();
            this->net.reset();
        }

        bool smartExec(RequestPtr &request, ResponsePtr &response, NetPtr &net) {
            this->request = request;
            this->response = response;
            this->net = net;

            auto ret = this->perform();

            this->reset(); // Reset pointers.
            return ret;
        }

        bool exec(zia::api::HttpDuplex &http) override {
            this->response = Response::fromBasicHttpDuplex(http);
            this->request = Request::fromBasicHttpDuplex(http);
            this->net = Net::fromBasicNetInfo(http.info);

            auto ret = this->perform();

            http.resp = this->response->toBasicHttpResponse();
            http.req = this->request->toBasicHttpRequest();
            http.info = this->net->toBasicNetInfo();

            this->reset(); // Reset pointers.
            return ret;
        }

        virtual bool perform() = 0;
    };

}
