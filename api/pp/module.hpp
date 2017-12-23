
#pragma one

#include "../module.h"

#include "conf.hpp"
#include "http.hpp"
#include "net.hpp"

namespace zia::apipp {

    class Module : public zia::api::Module {
    protected:
        Conf conf;
        std::unique_ptr<Response> response;
        std::unique_ptr<Request> request;
        Net net;

    public:
        ~Module() override = default;

        bool config(const zia::api::Conf &conf) override {
            this->conf = Conf::fromBasicConfig(conf);
            return true;
        }

        bool exec(zia::api::HttpDuplex &http) override {
            this->response = Response::fromBasicHttpResponse(http.resp);
            this->request = Request::fromBasicHttpRequest(http.req);
            this->net = Net::fromBasicNetInfo(http.info);

            auto ret = this->perform();

            http.resp = std::move(this->response->toBasicHttpResponse());
            http.req = std::move(this->request->toBasicHttpRequest());
            http.info = this->net.toBasicNetInfo();

            // We must be sure that we correctly releasing pointers
            this->response.release();
            this->request.release();

            return ret;
        }

        virtual bool perform() = 0;
    };

}
