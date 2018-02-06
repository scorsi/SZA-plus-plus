
#pragma once

#include "../conf.h"

/**
 * Wrapper of zia::api::Conf header to simplify the using.
 */
namespace zia::apipp {
    /**
     * Conf contains:
     * -> A variant elem
     * -> A type to better recognized the type behind the elem
     */
    struct ConfElem;
    struct ConfMap {
        std::map<std::string, ConfElem> elems;
    };
    struct ConfArray {
        std::vector<ConfElem> elems;
    };
    struct ConfElem {
        enum Type {
            Empty, // std::monostate
            Map, // ConfMap
            Array, // ConfArray
            String, // std::string
            Integer, // long long
            Double, // double
            Boolean // bool
        } type;
        std::variant<std::monostate, ConfMap, ConfArray, std::string, long long, double, bool> value;
    };

    class Conf {
    public:
        class InvalidAccess : std::exception {
        };

    private:
        ConfElem elem;

    public:
        Conf() = default;

        template<typename T>
        T get() {
            try {
                return std::get<T *>(elem.value);
            } catch (std::bad_variant_access &e) {
                return nullptr;
            }
        }

        ConfElem::Type getType() {
            return elem.type;
        }

        ConfElem &operator[](const int index) {
            try {
                return (std::get<ConfArray>(elem.value)).elems.at(index);
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
        }

        ConfElem &operator[](const std::string &index) {
            try {
                return (std::get<ConfMap>(elem.value)).elems.at(index);
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
        }

        static Conf fromBasicConfig(const zia::api::Conf &conf) {
            return {};
        }
    };
}