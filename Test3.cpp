//
// Created by scorsi on 08/02/18.
//

#include "api/pp/conf.hpp"

using namespace std::literals::string_literals;

using namespace zia::apipp;

void test3() {
    auto conf = (new ConfElem())
            ->set(new ConfMap)
            ->set_at("string_test", (new ConfElem())->set("first_value"s))
            ->set_at("integer_test", (new ConfElem())->set(42))
            ->set_at("double_test", (new ConfElem())->set(42.42))
            ->set_at("nested_map_test", (new ConfElem())
                    ->set(new ConfMap())
                    ->set_at("op", (new ConfElem())->set(1)))
            ->set_at("array_test", (new ConfElem())
                    ->set(new ConfArray())
                    ->push((new ConfElem())->set(true))
                    ->push((new ConfElem())->set(false)));

    // TODO: Better getter
    std::cout << (*conf)["string_test"]->get<std::string>() << std::endl;
    std::cout << (*conf)["integer_test"]->get<int>() << std::endl;
    std::cout << (*conf)["integer_test"]->get<long long>() << std::endl;
    std::cout << (*conf)["double_test"]->get<double>() << std::endl;
    std::cout << (*conf)["double_test"]->get<float>() << std::endl;

    std::cout << (*(*conf)["nested_map_test"])["op"]->get<int>() << std::endl;

    std::cout << (*(*conf)["array_test"])[0]->get<bool>() << std::endl;
    std::cout << (*(*conf)["array_test"])[1]->get<bool>() << std::endl;

    delete conf;
}