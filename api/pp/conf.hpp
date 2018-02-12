
#pragma once

#include <iostream>
#include "../conf.h"

/**
 * Wrapper of zia::api::Conf header to simplify the using.
 */
namespace zia::apipp {

    class ConfElem;

    struct ConfMap {
        std::map<std::string, ConfElem *> elems;

        // Put in the .cpp file to allow proper destructor call.
        ~ConfMap();
    };

    struct ConfArray {
        std::vector<ConfElem *> elems;

        // Put in the .cpp file to allow proper destructor call.
        ~ConfArray();
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
        std::variant<std::monostate, ConfMap *, ConfArray *, std::string, long long, double, bool> value;

    public:
        ConfElem() {
            type = Empty;
        }

        ~ConfElem();

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
            } catch (std::bad_variant_access &) {
                throw InvalidAccess();
            }
        }

        ConfElem *get_at(const int index) {
            return (*this)[index];
        }

        ConfElem *get_at(const std::string &index) {
            return (*this)[index];
        }

        /**
         * Set the value into the std::variant.
         *
         * If a invalid T is passed, a error (LNK2019 on MSVC) will occur during the linking pass of the compilation.
         *
         * @tparam T
         */
        template<typename T>
        ConfElem *set(T &&);

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
        ConfElem *push(ConfElem *val) {
            try {
                std::get<ConfArray *>(value)->elems.push_back(val);
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return this;
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
        ConfElem *set_at(const std::string &index, ConfElem *val) {
            try {
                std::get<ConfMap *>(value)->elems[index] = val;
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return this;
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
        ConfElem *operator[](const int index) {
            try {
                return (std::get<ConfArray *>(value))->elems.at(index);
            } catch (std::exception &) {
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
        ConfElem *operator[](const std::string &index) {
            try {
                return (std::get<ConfMap *>(value))->elems.at(index);
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
        }

        static ConfElem fromBasicConfig(const zia::api::Conf &conf) {
            return {};
        }
    };

    /**
     * Alias for long long.
     *
     * @return
     */
    template<>
    inline int ConfElem::get<int>() {
        return static_cast<int>(get<long long>());
    }

    /**
     * Alias for double.
     *
     * @return
     */
    template<>
    inline float ConfElem::get<float>() {
        return static_cast<float>(get<double>());
    }

    using Conf = ConfElem;
}