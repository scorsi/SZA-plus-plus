//
// Created by scorsi on 08/02/18.
//

#include "api/pp/conf.hpp"
#include "api/pp/visitor.hpp"

using namespace std::literals::string_literals;

using namespace zia::apipp;
void test3() {

    /// Don't need ConfElem explicit constructor call when setting values,
    /// But still need it when manipulating values afterwards (for ConfMap and
    /// ConfArray for example).
    auto conf = ConfElem()
        .set(ConfMap())
        .set_at("string_test", "first_value"s)
        .set_at("integer_test", 42)
        .set_at("double_test", 42.42)
        .set_at("nested_map_test", ConfElem(ConfMap())
            .set_at("op", 1))
        .set_at("array_test",
                ConfElem(ConfArray())
                    .push(true)
                    .push(false));

    std::cout << conf.get_at("string_test").get<std::string>() << std::endl;
    std::cout << conf.get_at("integer_test").get<int>() << std::endl;
    std::cout << conf.get_at("integer_test").get<long long>() << std::endl;
    std::cout << conf.get_at("double_test").get<double>() << std::endl;
    std::cout << conf.get_at("double_test").get<float>() << std::endl;

    std::cout << conf.get_at("nested_map_test").get_at("op").get<int>() << std::endl;

    std::cout << conf.get_at("array_test").get_at(0).get<bool>() << std::endl;
    std::cout << conf.get_at("array_test").get_at(1).get<bool>() << std::endl;

    std::cout << conf["array_test"][0].get<bool>() << std::endl;

    std::cout << conf << std::endl;

    /// Test with copy
    {
        std::cout << "TEST -- No copy" << std::endl;
        auto conf3 = std::make_shared<ConfElem>();
        conf3->set(ConfArray());

        auto conf4 = std::make_shared<ConfElem>();

        std::string v("toto");
        conf4->set(v);

        conf3->push(conf4);
        conf3->push(ConfElem().set("titi"s));

        /// Copy a previous config object but doesn't keep references.
        auto conf2 = conf;
        conf2.set(ConfMap()).set_at("data", conf3);
        conf2.set_at("string_test", "in_copied_map."s);

        conf4->set("new_value"s);

        std::cout << conf.get_at("string_test").get<std::string>() << std::endl;
        std::cout << conf2.get_at("string_test").get<std::string>() << std::endl;
        std::cout << conf2.get_at("data").get_at(0).get<std::string>() << std::endl;
        std::cout << conf2.get_at("data").get_at(1).get<std::string>() << std::endl;
    }

    /// Test with constructor helpers
    {
        std::cout << "TEST --  Contructor helpers." << std::endl;

        /// Works also without shared_ptr, but must be either moved or  built inline.
        auto confArray = std::make_shared<ConfElem>(ConfArray());
        std::cout << "ConfArray Type: " << confArray->getType() << std::endl;
        auto confString = std::make_shared<ConfElem>("toto"s);
        std::cout << "ConfString Type: " << confString->getType() << std::endl;
        auto confMap = std::make_shared<ConfElem>(ConfMap());
        std::cout << "ConfMap Type: " << confMap->getType() << std::endl;
        auto confInt = std::make_shared<ConfElem>(426879);
        std::cout << "ConfInt Type: " << confInt->getType() << std::endl;
        auto confLong = std::make_shared<ConfElem>(8566756747856);
        std::cout << "ConfLong Type: " << confLong->getType() << std::endl;
        auto confBool = std::make_shared<ConfElem>(true);
        std::cout << "ConfBool Type: " << confBool->getType() << std::endl;
        auto confFloat = std::make_shared<ConfElem>(42.42);
        std::cout << "ConfFloat Type: " << confFloat->getType() << std::endl;
        auto confDouble = std::make_shared<ConfElem>(42.42856756567);
        std::cout << "ConfDouble Type: " << confDouble->getType() << std::endl;

        confArray->push(confString);
        confArray->push("titi"s);

        auto conf2 = ConfElem().set(ConfMap()).set_at("data", confArray);
        confString->set("new_value"s);

        std::cout << conf2.get_at("data").get_at(0).get<std::string>() << std::endl;
        std::cout << conf2.get_at("data").get_at(1).get<std::string>() << std::endl;

        confMap->set_at("first", "Value1"s);
        confMap->set_at("second", "Value2"s);

        conf2.set_at("data", confMap);
        std::cout << conf2.get_at("data").get_at("first").get<std::string>() << std::endl;
    }

    /// Test prettify
    {
        zia::api::ConfValue valueBool;
        valueBool.v = true;
        zia::api::ConfValue valueString;
        valueString.v = "String Value"s;
        zia::api::ConfValue valueLong;
        valueLong.v = static_cast<long long int>(1785);
        zia::api::ConfValue valueDouble;
        valueDouble.v = 42.42;


        zia::api::ConfValue valueArray;
        valueArray.v = std::vector<zia::api::ConfValue> {valueBool, valueString, valueLong, valueDouble};

        zia::api::ConfObject valueMap;
        valueMap = std::map<std::string, zia::api::ConfValue> {
            {"first",  valueArray},
            {"second", valueBool},
            {"third",  valueLong},
            {"fourth", valueString},
            {"fifth",  valueDouble}
        };
        zia::api::Conf root = valueMap;

        std::cout << "TEST -- To SZA ++ Config" << std::endl;
        auto wrappedConfig = Conf::fromBasicConfig(root);
        std::cout << wrappedConfig << std::endl;

        std::cout << "TEST -- From SZA ++ Config" << std::endl;
        auto newConfig = wrappedConfig.toBasicConfig();
        std::cout << newConfig << std::endl;

        /// Visitor used to iterate over the configuration.
        /// Use this instead of chaining "get<T>()" calls with try/catch.
        /// The first argument is the "self" lambda. Call it to continue recursion.
        std::cout << "TEST -- Visit of Conf element" << std::endl;

        /// Without encapsulated visit.
        auto testVisitor = make_recursive_visitor<void>(
            [](auto, std::monostate) { std::cout << "Empty" << std::endl; },
            [](auto, std::string const &) { std::cout << "String" << std::endl; },
            [](auto, long long int) { std::cout << "Long" << std::endl; },
            [](auto, double) { std::cout << "Double" << std::endl; },
            [](auto, bool) { std::cout << "Bool" << std::endl; },
            [](auto recurse, ConfArray::Sptr const &array) {
                std::cout << "Array" << std::endl;
                for (auto &&v : array->elems) {
                    recurse(v->getValue());
                }
            },
            [](auto recurse, ConfMap::Sptr const &map) {
                std::cout << "Map" << std::endl;
                for (auto &&v : map->elems) {
                    recurse(v.second->getValue());
                }
            }
        );

        std::visit(testVisitor, wrappedConfig.getValue());

        /// With encapsulated visit. void is the default return type.
        wrappedConfig.visit(
            [](auto, std::monostate) { std::cout << "Empty" << std::endl; },
            [](auto, std::string const &) { std::cout << "String" << std::endl; },
            [](auto, long long int) { std::cout << "Long" << std::endl; },
            [](auto, double) { std::cout << "Double" << std::endl; },
            [](auto, bool) { std::cout << "Bool" << std::endl; },
            [](auto recurse, ConfArray::Sptr const &array) {
                std::cout << "Array" << std::endl;
                for (auto &&v : array->elems) {
                    recurse(v->getValue());
                }
            },
            [](auto recurse, ConfMap::Sptr const &map) {
                std::cout << "Map" << std::endl;
                for (auto &&v : map->elems) {
                    recurse(v.second->getValue());
                }
            });
    }
}