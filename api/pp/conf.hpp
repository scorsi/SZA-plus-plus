
#pragma once

#include <iostream>
#include <memory>
#include "../conf.h"

/**
 * Wrapper of zia::api::Conf header to simplify the using.
 */
namespace zia::apipp {

    class ConfElem;

    struct ConfMap {

        using Uptr = std::unique_ptr<ConfMap>;
        using Sptr = std::shared_ptr<ConfMap>;

        ConfMap() = default;
//        ConfMap(ConfMap const&) = default;
        ConfMap(ConfMap&&) = default;

  //      ConfMap& operator=(ConfMap const&) = default;
        ConfMap& operator=(ConfMap&&) = default;

        std::map<std::string, ConfElem> elems;

        // Put in the .cpp file to allow proper destructor call.
        ~ConfMap();
    };

    struct ConfArray {
        using Uptr = std::unique_ptr<ConfArray>;
        using Sptr = std::shared_ptr<ConfArray>;

        ConfArray() = default;
    //    ConfArray(ConfArray const&) = default;
        ConfArray(ConfArray&&) = default;

      //  ConfArray& operator=(ConfArray const&) = default;
        ConfArray& operator=(ConfArray&&) = default;

        std::vector<ConfElem> elems;

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
        std::variant<std::monostate, ConfMap::Sptr, ConfArray::Sptr, std::string, long long, double, bool> value;

    public:
        ConfElem() {
            type = Empty;
        }

        ConfElem(ConfElem const&) = default;
        ConfElem(ConfElem&&) = default;

        ConfElem& operator=(ConfElem const&) = default;
        ConfElem& operator=(ConfElem&&) = default;

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
        T get() const {
            try {
                return std::get<T>(value);
            } catch (std::bad_variant_access &) {
                throw InvalidAccess();
            }
        }

        ConfElem &get_at(const int index) {
            return (*this)[index];
        }

        const ConfElem &get_at(const int index) const {
            return (*this)[index];
        }

        ConfElem &get_at(const std::string &index) {
            return (*this)[index];
        }

        const ConfElem &get_at(const std::string &index) const {
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
        ConfElem &set(T &&) &;

        template<typename T>
        ConfElem &&set(T &&) &&;

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
        ConfElem &push(ConfElem &&val) & {
            try {
                std::get<ConfArray::Sptr>(value)->elems.emplace_back( std::move(val) ) ;
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return *this;
        }

        ConfElem &&push(ConfElem &&val) && {
            try {
                std::get<ConfArray::Sptr>(value)->elems.emplace_back( std::move(val) ) ;
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return std::move(*this);
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
        ConfElem &set_at(const std::string &index, ConfElem && val) & {
            try {
                std::get<ConfMap::Sptr>(value)->elems.emplace(index, std::move(val));
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return *this;
        }

        ConfElem &&set_at(const std::string &index, ConfElem && val) && {
            try {
                std::get<ConfMap::Sptr>(value)->elems.emplace(index, std::move(val));
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return std::move(*this);
        }

        ConfElem &set_at(const std::string &index, ConfElem const& val) & {
            try {
                std::get<ConfMap::Sptr>(value)->elems.emplace(index, val);
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return *this;
        }

        ConfElem &&set_at(const std::string &index, ConfElem const& val) && {
            try {
                std::get<ConfMap::Sptr>(value)->elems.emplace(index, val);
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
            return std::move(*this);
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
                return (std::get<ConfArray::Sptr>(value))->elems.at(index);
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
        }

        const ConfElem &operator[](const int index) const {
            try {
                return (std::get<ConfArray::Sptr>(value))->elems.at(index);
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
        ConfElem &operator[](const std::string &index) {
            try {
                return (std::get<ConfMap::Sptr>(value))->elems.at(index);
            } catch (std::exception &) {
//                std::cout << e.what() << std::endl;
                throw InvalidAccess();
            }
        }

        const ConfElem &operator[](const std::string &index) const {
            try {
                return (std::get<ConfMap::Sptr>(value))->elems.at(index);
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
    inline int ConfElem::get<int>() const {
        return static_cast<int>(get<long long>());
    }

    /**
     * Alias for double.
     *
     * @return
     */
    template<>
    inline float ConfElem::get<float>() const {
        return static_cast<float>(get<double>());
    }

    using Conf = ConfElem;
}