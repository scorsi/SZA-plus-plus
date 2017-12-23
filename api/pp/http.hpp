
#pragma once

#include <memory>
#include <sstream>
#include "../http.h"

namespace zia::apipp {

    class Response {
    private:
        bool useRawBody = false;

    public:
        const zia::api::http::Version version;
        std::map<std::string, std::vector<std::string>> headers;
        std::string body{};
        zia::api::Net::Raw rawBody;
        int statusCode{0};
        std::string statusReason{};

        Response(zia::api::http::Version &version, std::map<std::string, std::string> &headers,
                 zia::api::Net::Raw &rawBody, int statusCode, std::string &statusReason)
                : version{version}, rawBody{rawBody}, statusCode(statusCode), statusReason{statusReason} {

            for (const auto &item : headers) {
                std::stringstream ss(item.second);
                std::string res;
                while (std::getline(ss, res, ',')) {
                    this->headers[item.first].push_back(res);
                }
            }
        }

        Response *addHeader(const std::string &name, const std::string &value) {
            this->headers[name].push_back(value);
            return this;
        }

        Response *addHeaders(const std::string &name, const std::vector<std::string> &values) {
            for (const auto &item : values) {
                this->headers[name].push_back(item);
            }
            return this;
        }

        Response *removeAllHeadersByName(const std::string &name) {
            this->headers.erase(name);
            return this;
        }

        Response *setStatus(int code, const std::string &reason) {
            this->statusCode = code;
            this->statusReason = reason;
            return this;
        }

        static std::unique_ptr<Response> fromBasicHttpResponse(zia::api::HttpResponse &basicResponse) {
            auto response = std::make_unique<Response>(basicResponse.version, basicResponse.headers, basicResponse.body,
                                                       basicResponse.status, basicResponse.reason);

            std::transform(basicResponse.body.begin(), basicResponse.body.end(), std::back_inserter(response->body),
                           [](auto c) { return static_cast<char>(c); });

            return response;
        }

        zia::api::HttpResponse toBasicHttpResponse() {
            std::map<std::string, std::string> basicHeaders;

            for (const auto &item : this->headers) {
                for (const auto &value: item.second) {
                    basicHeaders[item.first] += value + ", ";
                }
                auto &tmp = basicHeaders[item.first];
                basicHeaders[item.first] = tmp.substr(0, tmp.length() - 2);
            }

            if (this->useRawBody) {
                return zia::api::HttpResponse{this->version, basicHeaders, this->rawBody,
                                              this->statusCode, this->statusReason};
            } else {
                std::vector<std::byte> basicBody;

                std::transform(this->body.begin(), this->body.end(), std::back_inserter(basicBody),
                               [](auto c) { return static_cast<std::byte>(c); });

                return zia::api::HttpResponse{this->version, basicHeaders, basicBody,
                                              this->statusCode, this->statusReason};
            }
        }
    };

    class Request {
    private:
        bool useRawBody = false;

    public:
        const zia::api::http::Version version;
        std::map<std::string, std::vector<std::string>> headers;
        std::string body{};
        zia::api::Net::Raw rawBody;
        const zia::api::http::Method method;
        const std::string uri;

        Request(zia::api::http::Version version, std::map<std::string, std::string> &headers,
                zia::api::Net::Raw &rawBody, zia::api::http::Method method, std::string &uri)
                : version{version}, rawBody{rawBody}, headers{}, method{method}, uri{uri} {

            for (const auto &item : headers) {
                std::stringstream ss(item.second);
                std::string res;
                while (std::getline(ss, res, ',')) {
                    this->headers[item.first].push_back(res);
                }
            }
        }

        static std::unique_ptr<Request> fromBasicHttpRequest(zia::api::HttpRequest &basicRequest) {
            auto request = std::make_unique<Request>(basicRequest.version, basicRequest.headers, basicRequest.body,
                                                     basicRequest.method, basicRequest.uri);

            std::transform(basicRequest.body.begin(), basicRequest.body.end(), std::back_inserter(request->body),
                           [](auto c) { return static_cast<char>(c); });

            return request;
        }

        zia::api::HttpRequest toBasicHttpRequest() {
            std::map<std::string, std::string> basicHeaders;

            for (const auto &item : this->headers) {
                for (const auto &value: item.second) {
                    basicHeaders[item.first] += value + ", ";
                }
                auto &tmp = basicHeaders[item.first];
                basicHeaders[item.first] = tmp.substr(0, tmp.length() - 2);
            }

            if (this->useRawBody) {
                return zia::api::HttpRequest{this->version, basicHeaders, this->rawBody, this->method, this->uri};
            } else {
                std::vector<std::byte> basicBody;

                std::transform(this->body.begin(), this->body.end(), std::back_inserter(basicBody),
                               [](auto c) { return static_cast<std::byte>(c); });

                return zia::api::HttpRequest{this->version, basicHeaders, basicBody, this->method, this->uri};
            }
        }
    };

}
