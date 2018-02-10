//
// Created by sylva on 10/02/2018.
//

#include "conf.hpp"

namespace zia::apipp {

    apipp::ConfMap::~ConfMap() {
        for (auto &item : elems) {
            delete item.second;
        }
    }

    ConfArray::~ConfArray() {
        for (auto &item : elems) {
            delete item;
        }
    }

    ConfElem::~ConfElem() {
        if (type == Map) {
            delete std::get<ConfMap *>(value);
        } else if (type == Array) {
            delete std::get<ConfArray *>(value);
        }
    }
}
