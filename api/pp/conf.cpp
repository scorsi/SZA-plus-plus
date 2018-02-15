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
            [](auto, auto value) { return ConfElem(value); },
            [](auto, std::string const& value) { return ConfElem(value); },

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

    std::ostream &operator<<(std::ostream &os, zia::apipp::Conf const &conf) {
        int sp {0};

        constexpr auto indent = [](int indent) -> std::string {
            return std::string(static_cast<std::size_t>(indent), ' ');
        };

        auto pvisit = make_recursive_visitor<void>( [&os, &sp, &indent](auto recurse, auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::monostate>) {}
            else if constexpr (std::is_same_v<T, long long>) {
                os << value;
            }
            else if constexpr (std::is_same_v<T, double>) {
                os << std::fixed << value;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                os << std::boolalpha << value;
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                os << std::quoted(value);
            }
            else if constexpr (std::is_same_v<T, ConfMap::Sptr>) {
                os << "{";
                sp += 4;
                for (auto endSep = --value->elems.end(), it = value->elems.begin(); it != value->elems.end() ; ++it) {
                    if (it == value->elems.begin())
                        os << '\n';
                    os << indent(sp) << std::quoted(it->first) <<  ": ";
                    recurse(it->second->getValue());
                    if (it != endSep)
                        os << ',';
                    os << '\n';
                }
                sp -= 4;
                os << indent(sp) << "}";
            }
            else if constexpr (std::is_same_v<T, ConfArray::Sptr>) {
                os << '[';
                sp += 4;
                auto it = value->elems.begin();

                for (auto end = value->elems.end(); it != end; ++it) {
                    if (it == value->elems.begin())
                        os << '\n';
                    os << indent(sp);
                    recurse((*it)->getValue());
                    if (it != end - 1)
                        os << ',';
                    os << '\n';
                }
                sp -= 4;
                os << indent(sp) << "]";
            }
        });
        std::visit(pvisit, conf.getValue());
        return os;
    }

    std::ostream &operator<<(std::ostream &os, zia::api::Conf const &conf) {
        int sp {0};

        constexpr auto indent = [](int indent) -> std::string {
            return std::string(static_cast<std::size_t>(indent), ' ');
        };

        auto mapPrettify = [&os, &sp, &indent] (auto recurse, zia::api::ConfObject value) {
            os << "{";
            sp += 4;
            for (auto endSep = --value.end(), it = value.begin(); it != value.end() ; ++it) {
                if (it == value.begin())
                    os << '\n';
                os << indent(sp) << std::quoted(it->first) <<  ": ";
                recurse(it->second.v);
                if (it != endSep)
                    os << ',';
                os << '\n';
            }
            sp -= 4;
            os << indent(sp) << "}";
        };

        auto pvisit = make_recursive_visitor<void>(
            [&os] (auto, std::monostate) { },
            [&os] (auto, long long value) { os << value; },
            [&os] (auto, double value) { os << std::fixed << value; },
            [&os] (auto, bool value) { os << std::boolalpha << value; },
            [&os] (auto, const std::string & value) { os << std::quoted(value); },
            [&os, &sp, &indent](auto recurse, zia::api::ConfArray value) {
                os << '[';
                sp += 4;
                auto it = value.begin();

                for (auto end = value.end(); it != end; ++it) {
                    if (it == value.begin())
                        os << '\n';
                    os << indent(sp);
                    recurse(it->v);
                    if (it != end - 1)
                        os << ',';
                    os << '\n';
                }
                sp -= 4;
                os << indent(sp) << ']';
            },
            mapPrettify
        );

        zia::api::ConfValue begin;
        begin.v = conf;

        std::visit(pvisit, begin.v);
        return os;
    }
}
