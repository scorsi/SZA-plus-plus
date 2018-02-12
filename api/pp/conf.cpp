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

  /**
  * Set a ConfMap value.
  *
  * @param val
  */
  template<>
  ConfElem *ConfElem::set<ConfMap *>(ConfMap *&&val) {
      type = Map;
      value = std::forward<ConfMap *>(val);
      return this;
  }

  /**
   * Set a ConfArray value.
   *
   * @param val
   */
  template<>
  ConfElem *ConfElem::set<ConfArray *>(ConfArray *&&val) {
      type = Array;
      value = std::forward<ConfArray *>(val);
      return this;
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
  ConfElem *ConfElem::set<char *>(char *&&val) {
      type = String;
      value = std::string(std::forward<char *>(val));
      return this;
  }

  /**
   * Set a std::string value.
   *
   * @param val
   */
  template<>
  ConfElem *ConfElem::set<std::string>(std::string &&val) {
      type = String;
      value = std::forward<std::string>(val);
      return this;
  }

  /**
   * Set a long long value.
   * Alias for long long.
   *
   * @param val
   */
  template<>
  ConfElem *ConfElem::set<int>(int &&val) {
      type = Integer;
      value = static_cast<long long>(std::forward<int>(val));
      return this;
  }

  /**
   * Set a long long value.
   *
   * @param val
   */
  template<>
  ConfElem *ConfElem::set<long long>(long long &&val) {
      type = Integer;
      value = std::forward<long long>(val);
      return this;
  }

  /**
   * Set a double value.
   * Alias for double.
   *
   * @param val
   */
  template<>
  ConfElem *ConfElem::set<float>(float &&val) {
      type = Integer;
      value = static_cast<double>(std::forward<float>(val));
      return this;
  }

  /**
   * Set a double value.
   *
   * @param val
   */
  template<>
  ConfElem *ConfElem::set<double>(double &&val) {
      type = Double;
      value = std::forward<double>(val);
      return this;
  }

  /**
   * Set a bool value.
   *
   * @param val
   */
  template<>
  ConfElem *ConfElem::set<bool>(bool &&val) {
      type = Boolean;
      value = std::forward<bool>(val);
      return this;
  }
}
