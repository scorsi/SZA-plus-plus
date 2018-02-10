
#pragma once

#include <iostream>
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
    class ConfElem;

    struct ConfMap {
        std::map<std::string, ConfElem> elems;
    };
    struct ConfArray {
        std::vector<ConfElem> elems;
    };

    class ConfElem {
    public:
        enum Type {
            Empty, // std::monostate
            Map, // ConfMap
            Array, // ConfArray
            String, // std::string
            Integer, // long long
            Double, // double
            Boolean // bool
        };

        class InvalidAccess : std::exception {
        };

    private:
        Type type;
        std::variant<std::monostate, ConfMap, ConfArray, std::string, long long, double, bool> value;

    public:
        ConfElem() {
            type = ConfElem::Empty;
        }

        /**
         * Get the value stored in the std::variant.
         *
         * If a invalid T is passed, a error (C2338 on MSVC) will occur.
         * Don't worry. You surely typed "int" instead of "long long".
         *
         * @tparam T
         * @return
         */
        template<typename T>
        T get() {
            try {
                return std::get<T>(value);
            } catch (std::bad_variant_access &e) {
                throw InvalidAccess();
            }
        }

        /**
         * Alias for long long.
         *
         * @return
         */
        template<>
        int get<int>() {
            try {
                return static_cast<int>(std::get<long long>(value));
            } catch (std::bad_variant_access &e) {
                throw InvalidAccess();
            }
        }

        /**
         * Alias for double.
         *
         * @return
         */
        template<>
        float get<float>() {
            try {
                return static_cast<float>(std::get<double>(value));
            } catch (std::bad_variant_access &e) {
                throw InvalidAccess();
            }
        }

        /**
         * Set the value into the std::variant.
         *
         * If a invalid T is passed, a error (LNK2019 on MSVC) will occur during the linking pass of the compilation.
         *
         * @tparam T
         */
        template<typename T>
        ConfElem &set(T &&);

        /**
         * Set a ConfMap value.
         *
         * @param val
         */
        template<>
        ConfElem &set<ConfMap>(ConfMap &&val) {
            type = ConfElem::Map;
            value = std::forward<ConfMap>(val);
            return *this;
        }

        /**
         * Set a ConfArray value.
         *
         * @param val
         */
        template<>
        ConfElem &set<ConfArray>(ConfArray &&val) {
            type = ConfElem::Array;
            value = std::forward<ConfArray>(val);
            return *this;
        }

        /**
         * Set a std::string value.
         * Alias for std::string.
         *
         * TODO: Fix following bug
         * @bug when using like : conf.set("toto"). Possible fix: using the string_literal suffixe to use std::string instead of char *.
         * @param val
         */
        template<>
        ConfElem &set<char *>(char *&&val) {
            type = ConfElem::String;
            value = std::string(std::forward<char *>(val));
            return *this;
        }

        /**
         * Set a std::string value.
         *
         * @param val
         */
        template<>
        ConfElem &set<std::string>(std::string &&val) {
            type = ConfElem::String;
            value = std::forward<std::string>(val);
            return *this;
        }

        /**
         * Set a long long value.
         * Alias for long long.
         *
         * @param val
         */
        template<>
        ConfElem &set<int>(int &&val) {
            type = ConfElem::Integer;
            value = static_cast<long long>(std::forward<int>(val));
            return *this;
        }

        /**
         * Set a long long value.
         *
         * @param val
         */
        template<>
        ConfElem &set<long long>(long long &&val) {
            type = ConfElem::Integer;
            value = std::forward<long long>(val);
            return *this;
        }

        /**
         * Set a double value.
         * Alias for double.
         *
         * @param val
         */
        template<>
        ConfElem &set<float>(float &&val) {
            type = ConfElem::Integer;
            value = static_cast<double>(std::forward<float>(val));
            return *this;
        }

        /**
         * Set a double value.
         *
         * @param val
         */
        template<>
        ConfElem &set<double>(double &&val) {
            type = ConfElem::Double;
            value = std::forward<double>(val);
            return *this;
        }

        /**
         * Set a bool value.
         *
         * @param val
         */
        template<>
        ConfElem &set<bool>(bool &&val) {
            type = ConfElem::Boolean;
            value = std::forward<bool>(val);
            return *this;
        }

        /**
         * Push a ConfElem into the ConfArray value.
         *
         * @throw InvalidAccess if the value is not a ConfArray
         *
         * @tparam T
         * @param index
         * @param val
         * @return
         */
        ConfElem &push(ConfElem &val) {
            try {
                std::get<ConfArray>(value).elems.push_back(val);
            } catch (std::exception &e) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return *this;
        }

        /**
         * Insert a value into the ConfMap value.
         *
         * @throw InvalidAccess if the value is not a ConfMap.
         *
         * @param index
         * @param val
         * @return
         */
        ConfElem &set_at(const std::string &index, ConfElem &val) {
            try {
                std::get<ConfMap>(value).elems[index] = val;
            } catch (std::exception &e) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return *this;
        }

        /**
         * Get the enum type of the variant value.
         *
         * @return
         */
        ConfElem::Type getType() {
            return type;
        }

        /**
         * Access to an index of a ConfArray.
         *
         * @throw InvalidAccess if the value is not a ConfArray
         *
         * @param index
         * @return
         */
        ConfElem &operator[](const int index) {
            try {
                return (std::get<ConfArray>(value)).elems.at(index);
            } catch (std::exception &e) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
        }

        /**
         * Access to an index of a ConfMap.
         *
         * @throw InvalidAccess if the value is not a ConfMap
         *
         * @param index
         * @return
         */
        ConfElem &operator[](const std::string &index) {
            try {
                return (std::get<ConfMap>(value)).elems.at(index);
            } catch (std::exception &e) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
        }

        static ConfElem fromBasicConfig(const zia::api::Conf &conf) {
            return {};
        }
    };

    using Conf = ConfElem;
}