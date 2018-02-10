//
// Created by scorsi on 08/02/18.
//

#include "api/pp/conf.hpp"

using namespace std::literals::string_literals;

using namespace zia::apipp;

void test3() {
    auto conf = Conf();
    conf.set(ConfMap());

    conf.set_at("string_test", Conf().set("first_value"s));
    conf.set_at("integer_test", Conf().set(42));
    conf.set_at("double_test", Conf().set(42.42));
    auto nconf = Conf().set(ConfMap());
    nconf.set_at("op", Conf().set(1));
    conf.set_at("nested_map_test", nconf);

    conf.set_at("array_test", Conf().set(ConfArray()));
    conf["array_test"].push(Conf().set(true));
    conf["array_test"].push(Conf().set(false));

    std::cout << conf["string_test"].get<std::string>() << std::endl;
    std::cout << conf["integer_test"].get<int>() << std::endl;
    std::cout << conf["integer_test"].get<long long>() << std::endl;
    std::cout << conf["double_test"].get<double>() << std::endl;
    std::cout << conf["double_test"].get<float>() << std::endl;

    std::cout << conf["nested_map_test"]["op"].get<int>() << std::endl;

    std::cout << conf["array_test"][0].get<bool>() << std::endl;
    std::cout << conf["array_test"][1].get<bool>() << std::endl;
}