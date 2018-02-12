//
// Created by sylva on 10/02/2018.
//

#include "conf.hpp"

namespace zia::apipp {

    apipp::ConfMap::~ConfMap() = default;

    ConfArray::~ConfArray() = default;

    ConfElem::~ConfElem() = default;

  /**
  * Set a ConfMap value.
  *
  * @param val
  */
  template<>
  ConfElem &ConfElem::set<ConfMap>(ConfMap &&val) & {
      type = Map;
      value = std::make_shared<ConfMap>(std::move(val));
      return *this;
  }

  template<>
  ConfElem &&ConfElem::set<ConfMap>(ConfMap &&val) && {
      type = Map;
      value = std::make_shared<ConfMap>(std::move(val));
      return std::move(*this);
  }

  /**
  * Set a ConfArray value.
  *
  * @param val
  */

  template<>
  ConfElem &ConfElem::set<ConfArray>(ConfArray &&val) & {
      type = Array;
      value = std::make_shared<ConfArray>(std::move(val));
      return *this;
  }

  template<>
  ConfElem &&ConfElem::set<ConfArray>(ConfArray &&val) && {
      type = Array;
      value = std::make_shared<ConfArray>(std::move(val));
      return std::move(*this);
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
  ConfElem &ConfElem::set<char *>(char *&&val) & {
      type = String;
      value = std::string(val);
      return *this;
  }

  template<>
  ConfElem &&ConfElem::set<char *>(char *&&val) && {
      type = String;
      value = std::string(val);
      return std::move(*this);
  }

  /**
   * Set a std::string value.
   *
   * @param val
   */
  template<>
  ConfElem &ConfElem::set<std::string>(std::string &&val) & {
      type = String;
      value = std::move(val);
      return *this;
  }

  template<>
  ConfElem &&ConfElem::set<std::string>(std::string &&val) && {
      type = String;
      value = std::move(val);
      return std::move(*this);
  }

  /**
   * Set a long long value.
   * Alias for long long.
   *
   * @param val
   */
  template<>
  ConfElem &ConfElem::set<int>(int &&val) & {
      type = Integer;
      value = static_cast<long long>(val);
      return *this;
  }

  template<>
  ConfElem &&ConfElem::set<int>(int &&val) && {
      type = Integer;
      value = static_cast<long long>(val);
      return std::move(*this);
  }

  /**
   * Set a long long value.
   *
   * @param val
   */
  template<>
  ConfElem &ConfElem::set<long long>(long long &&val) & {
      type = Integer;
      value = val;
      return *this;
  }

    template<>
    ConfElem &&ConfElem::set<long long>(long long &&val) && {
        type = Integer;
        value = val;
        return std::move(*this);
    }


    /**
   * Set a double value.
   * Alias for double.
   *
   * @param val
   */
    template<>
    ConfElem &ConfElem::set<float>(float &&val) & {
        type = Integer;
        value = static_cast<double>(val);
        return *this;
    }

  template<>
  ConfElem &&ConfElem::set<float>(float &&val) && {
      type = Integer;
      value = static_cast<double>(val);
      return std::move(*this);
  }

  /**
   * Set a double value.
   *
   * @param val
   */
  template<>
  ConfElem &ConfElem::set<double>(double &&val) & {
      type = Double;
      value = val;
      return *this;
  }

  template<>
  ConfElem &&ConfElem::set<double>(double &&val) && {
      type = Double;
      value = val;
      return std::move(*this);
  }

  /**
   * Set a bool value.
   *
   * @param val
   */
  template<>
  ConfElem &ConfElem::set<bool>(bool &&val) & {
      type = Boolean;
      value = val;
      return *this;
  }

  template<>
  ConfElem &&ConfElem::set<bool>(bool &&val) && {
      type = Boolean;
      value = val;
      return std::move(*this);
  }
}
