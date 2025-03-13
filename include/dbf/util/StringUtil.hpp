#ifndef STRING_UTIL_HPP
#define STRING_UTIL_HPP

#include <string>
#include <array>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cassert>
#include <type_traits>

#include <spdlog/spdlog.h>
#include <boost/utility/string_view.hpp>
#include <boost/lexical_cast.hpp>

namespace util {
template <size_t TotalSize>
inline std::string toString(const std::array<char, TotalSize> &arr) {
  return std::string(arr.data(), arr.size());
}

inline std::string toString(char dst) { return std::string(1, dst); }

template <typename T> inline bool stringToInt(const std::string &str, T &t) {
  try {
    t = static_cast<T>(std::stoll(str));
    return true;
  } catch (const std::exception &ex) {
    SPDLOG_WARN("str : {}, exception {}", str, ex.what());
    return false;
  }
}

template <typename T> inline T stringToInt(const std::string &str) {
  try {
    return static_cast<T>(std::stoll(str));
  } catch (const std::exception &ex) {
    SPDLOG_WARN("str : {}, exception {}", str, ex.what());
    return 0;
  }
}

template <size_t TotalSize, size_t PrecisionSize, typename T>
inline T floatStringToInt(const std::string &str) {
  if (PrecisionSize == 0) {
    return stringToInt<T>(str);
  }
  const auto pointIter = std::find(str.begin(), str.end(), '.');
  if (pointIter == str.end()) {
    T value = stringToInt<T>(str);
    value *= static_cast<int32_t>(std::pow(10, PrecisionSize));
    return value;
  } else {
    std::array<char, TotalSize + 1> arr;
    arr.fill('0');
    std::copy(str.begin(), pointIter, arr.data());
    std::copy(pointIter + 1, str.end(), arr.data() + (pointIter - str.begin()));
    arr[pointIter - str.begin() + PrecisionSize] = 0;
    return stringToInt<T>(std::string(arr.data()));
  }
}

template <size_t MaxSize, size_t PrecisionSize, typename T>
inline std::string intToFloatString(T val) {
  std::string result = std::to_string(val);
  if (PrecisionSize == 0) {
    return result;
  }

  if (val >= 0) {
    if (result.length() <= PrecisionSize) {
      result.insert(0, PrecisionSize + 1 - result.length(), '0');
    }
  } else {
    if (result.length() <= PrecisionSize + 1) {
      result.insert(1, PrecisionSize + 2 - result.length(), '0');
    }
  }

  if (PrecisionSize > 0) {
    result.insert(result.length() - PrecisionSize, ".");
  }

  return result;
}


template <typename T>
inline bool intToStringView(T t, boost::string_view &view) {
  int64_t index = view.size() - 1;
  if (t == 0) {
    *const_cast<char *>(view.data() + index) = '0';
    view = view.substr(index, view.size() - index);
    return true;
  }

  int64_t value = std::abs((int64_t)t);
  while (value > 0) {
    if (index >= 0) {
      *const_cast<char *>(view.data() + index) = value % 10 + '0';
      --index;
      value /= 10;
    } else {
      SPDLOG_WARN("string_view size to small, {}", t);
      return false;
    }
  }

  if (t < 0) {
    if (index >= 0) {
      *const_cast<char *>(view.data() + index) = '-';
      --index;
    } else {
      SPDLOG_WARN("string_view size to small, {}", t);
      return false;
    }
  }
  view = view.substr(index + 1, view.size() - (index + 1));
  return true;
}

template <typename T> inline T stringViewToInt(const boost::string_view &view) {
  if (view.empty()) {
    return 0;
  }
  try {
    T value(0);
    if (view[0] == '-') {
      for (size_t index = 1; index != view.size(); ++index) {
        value = value * 10 + view[index] - '0';
      }
      return 0 - value;
    } else {
      for (size_t index = 0; index != view.size(); ++index) {
        value = value * 10 + view[index] - '0';
      }
      return value;
    }
  } catch (const std::exception &ex) {
    SPDLOG_WARN("str : {}, exception {}", std::string(view.data(), view.size()),
                ex.what());
    return 0;
  }
}


template <typename T>
inline bool stringViewToInt(const boost::string_view &view, T &value) {
  value = 0;
  try {
    if (view[0] == '-') {
      for (size_t index = 1; index != view.size(); ++index) {
        value = value * 10 + view[index] - '0';
      }
      value = 0 - value;
    } else {
      for (size_t index = 0; index != view.size(); ++index) {
        value = value * 10 + view[index] - '0';
      }
    }
    return true;
  } catch (const std::exception &ex) {
    SPDLOG_WARN("str : {}, exception {}", std::string(view.data(), view.size()),
                ex.what());
    return false;
  }
}

template <size_t TotalSize, size_t PrecisionSize, typename T>
inline T floatStringViewToInt(const boost::string_view &view) {
  static int64_t pow10[10] = {1,      10,      100,      1000,      10000,
                              100000, 1000000, 10000000, 100000000, 1000000000};
  if (PrecisionSize == 0) {
    return stringViewToInt<T>(view);
  }
  const auto pointIter = std::find(view.begin(), view.end(), '.');
  if (pointIter == view.end()) {
    T value = stringViewToInt<T>(view);
    value *= static_cast<int32_t>(pow10[PrecisionSize]);
    return value;
  } else {
    std::array<char, TotalSize + 1> arr;
    arr.fill('0');
    std::copy(view.begin(), pointIter, arr.data());
    std::copy(pointIter + 1, view.end(),
              arr.data() + (pointIter - view.begin()));
    int64_t size = pointIter - view.begin() + PrecisionSize;
    arr[size] = 0;
    return stringViewToInt<T>(boost::string_view(arr.data(), size));
  }
}

template <size_t MaxSize, size_t PrecisionSize, typename T>
inline bool intToFloatStringView(T val, boost::string_view &view) {
  static int64_t pow10[10] = {1,      10,      100,      1000,      10000,
                              100000, 1000000, 10000000, 100000000, 1000000000};
  if (PrecisionSize == 0) {
    return intToStringView(val, view);
  }

  int64_t multiple = pow10[PrecisionSize];
  int64_t value = std::abs((int64_t)val);
  int64_t integerPart = value / multiple;
  int64_t floatPart = value % multiple;

  int64_t index = view.size() - 1;
  while (floatPart > 0) {
    if (index >= 0) {
      *const_cast<char *>(view.data() + index) = floatPart % 10 + '0';
      --index;
      floatPart /= 10;
    } else {
      SPDLOG_WARN("string_view size to small, {}", val);
      return false;
    }
  }

  while (view.size() - 1 - index < PrecisionSize) {
    if (index > 0) {
      *const_cast<char *>(view.data() + index) = '0';
      --index;
    } else {
      SPDLOG_WARN("string_view size to small, {}", val);
      return false;
    }
  }
  if (index > 0) {
    *const_cast<char *>(view.data() + index) = '.';
    --index;
  }

  if (integerPart == 0) {
    *const_cast<char *>(view.data() + index) = '0';
    --index;
  } else {
    while (integerPart > 0) {
      if (index >= 0) {
        *const_cast<char *>(view.data() + index) = integerPart % 10 + '0';
        --index;
        integerPart /= 10;
      } else {
        SPDLOG_WARN("string_view size to small, {}", val);
        return false;
      }
    }
  }

  if (val < 0) {
    if (index >= 0) {
      *const_cast<char *>(view.data() + index) = '-';
      --index;
    } else {
      SPDLOG_WARN("string_view size to small, {}", val);
      return false;
    }
  }
  view = view.substr(index + 1, view.size() - (index + 1));
  return true;
}

}

#endif // !STRING_UTIL_HPP
