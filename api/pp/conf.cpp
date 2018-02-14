//
// Created by sylva on 10/02/2018.
//

#include <iomanip>
#include "conf.hpp"
#include "visitor.hpp"

namespace zia::apipp {

    apipp::ConfMap::~ConfMap() = default;

    ConfArray::~ConfArray() = default;

    ConfElem::~ConfElem() = default;

    ConfElem ConfElem::fromBasicConfig(const zia::api::Conf &conf) {

        auto jsonConfig = ConfElem(ConfMap());

        auto visitor = make_recursive_visitor<ConfElem>(
            [](auto, std::monostate) { return ConfElem(); },

            [](auto, double value) { return ConfElem(value); },
            [](auto, long long value) { return ConfElem(value); },
            [](auto, std::string const& value) { return ConfElem(value); },
            [](auto, bool value) { return ConfElem(value); },

            [](auto self, zia::api::ConfObject const& map) {
                auto currentElem = ConfElem(ConfMap());
                for (auto& v : map)
                {
                    currentElem.set_at(v.first, self(v.second.v));
                }
                return currentElem;
            },

            [](auto self, zia::api::ConfArray const& array) {
                auto currentElem = ConfElem(ConfArray());

                for (auto& v : array) {
                    currentElem.push(self(v.v));
                }
                return currentElem;
            });

        for (auto const& basicvalue : conf)
        {
            jsonConfig.set_at(basicvalue.first, std::visit(visitor, basicvalue.second.v));
        }
        return jsonConfig;
    }

    zia::api::Conf ConfElem::toBasicConfig() {
        namespace wrapped = zia::api;
        zia::api::Conf basicConf {};

        auto pvisit = make_recursive_visitor<wrapped::ConfValue>(
            [](auto, std::monostate) { return wrapped::ConfValue(); },
            [](auto, auto value) {
                auto v = wrapped::ConfValue();
                v.v = value;
                return v;
            },
            [](auto, std::string const& value) {
                auto v = wrapped::ConfValue();
                v.v = value;
                return v;
            },
            [](auto recurse, ConfArray::Sptr const& array) {
                wrapped::ConfArray basicarray {};

                for (auto&& value : array->elems)
                    basicarray.push_back(recurse(value->getValue()));

                wrapped::ConfValue value;
                value.v = basicarray;
                return value;
            },
            [](auto recurse, ConfMap::Sptr const& map) {
                wrapped::ConfObject basicmap {};

                for (auto&& value : map->elems) {
                    basicmap.insert({value.first, recurse(value.second->getValue()) });
                }

                wrapped::ConfValue value;
                value.v = basicmap;
                return value;
            }
        );

        auto root = std::visit(pvisit, value);
        if (type == Map) {
            return std::get<wrapped::ConfObject>(root.v);
        }
        else {
            return wrapped::ConfObject { { "data", root} };
        }
    }

    std::string ConfElem::basicPrettify(const zia::api::Conf &conf) {
        std::ostringstream os {};
        int sp {0};

        constexpr auto indent = [](int indent) -> std::string {
            return std::string(static_cast<std::size_t>(indent), ' ');
        };

        auto mapPrettify = [&os, &sp, &indent] (auto recurse, zia::api::ConfObject value) {
            os << "{\n";
            sp += 4;
            for (auto&& v : value) {
                os << indent(sp) << std::quoted(v.first) <<  ": ";
                recurse(v.second.v);
                os << ",\n";
            }
            sp -= 4;
            os << indent(sp) << "}";
        };


        auto pvisit = make_recursive_visitor<void>(
            [&os] (auto, std::monostate) { },
            [&os] (auto, long long value) { os << value; },
            [&os] (auto, double value) { os << std::fixed << value; },
            [&os] (auto, bool value) { os << std::boolalpha << value; },
            [&os] (auto, const std::string & value) { os << value; },
            [&os, &sp, &indent](auto recurse, zia::api::ConfArray value) {
                os << "[\n";
                sp += 4;
                auto it = value.begin();
                if (it != value.end())
                {
                    os << indent(sp);
                    recurse(it->v);
                }
                for (auto end = value.end(); it != end; ++it) {
                    os << ",\n" << indent(sp);
                    recurse(it->v);
                }
                sp -= 4;
                os << "\n" << indent(sp) << "]";
            },
            mapPrettify
        );

        zia::api::ConfValue begin;
        begin.v = conf;

        std::visit(pvisit, begin.v);
        return os.str();

    }

    std::string ConfElem::prettify(const ConfElem &conf) {
        std::ostringstream os {};
        int sp {0};

        constexpr auto indent = [](int indent) -> std::string {
            return std::string(static_cast<std::size_t>(indent), ' ');
        };

        auto pvisit = make_recursive_visitor<void>(
            [&os] (auto, std::monostate) { },
            [&os] (auto, long long value) { os << value; },
            [&os] (auto, double value) { os << std::fixed << value; },
            [&os] (auto, bool value) { os << std::boolalpha << value; },
            [&os] (auto, const std::string & value) { os << value; },
            [&os, &sp, &indent] (auto recurse, const ConfMap::Sptr& map ) {
                os << "{\n";
                sp += 4;
                for (auto&& v : map->elems) {
                    os << indent(sp) << std::quoted(v.first) <<  ": ";
                    recurse(v.second->getValue());
                    os << ",\n";
                }
                sp -= 4;
                os << indent(sp) << "}";
            },
            [&os, &sp, &indent] (auto recurse, const ConfArray::Sptr& array) {
                os << "[\n";
                sp += 4;
                auto it = array->elems.begin();
                if (it != array->elems.end())
                {
                    os << indent(sp);
                    recurse((*it)->getValue());
                }
                for (auto end = array->elems.end(); it != end; ++it) {
                    os << ",\n" << indent(sp);
                    recurse((*it)->getValue());
                }
                sp -= 4;
                os << "\n" << indent(sp) << "]";
            }
        );
        std::visit(pvisit, conf.getValue());
        return os.str();
    }
}
