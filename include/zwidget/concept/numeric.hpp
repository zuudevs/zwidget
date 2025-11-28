#pragma once

#include <type_traits>

namespace zuu::_meta {

template <typename T>
concept Numeric = std::is_arithmetic_v<T> ;

} // namespace zuu::_meta
