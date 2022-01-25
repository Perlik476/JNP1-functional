#ifndef TRI_LIST_H
#define TRI_LIST_H

#include <vector>
#include <variant>
#include <concepts>
#include <iostream>
#include <numbers>
#include <string>
#include <cassert>
#include <algorithm>
#include <ranges>
#include <limits>
#include <iterator>
#include <ranges>
#include <functional>
#include "tri_list_concepts.h"

template <typename T>
T identity(T t) {
    return t;
}

template <typename T, modifier<T> M1, modifier<T> M2>
modifier<T> auto compose (M1 f1, M2 f2) {
    return [=](T t) {
        return f1(f2(t));
    };
}

template <typename T1, typename T2, typename T3>
class tri_list {
    using variant = std::variant<T1, T2, T3>;

    std::vector<variant> list;

    std::function<T1(T1)> T1_modifier = identity<T1>();
    std::function<T2(T2)> T2_modifier = identity<T2>();
    std::function<T3(T3)> T3_modifier = identity<T3>();

    template<class T>
    auto range_over() {
        auto result = list
            | std::views::filter([](variant el) {
                return std::holds_alternative<T>(el);
            })
            | std::views::transform([](variant el) {
                return std::get<T>(el);
            });

        if constexpr (std::is_same<T, T1>::value) {
            return result | std::views::transform(T1_modifier);
        }
        else if constexpr (std::is_same<T, T2>::value) {
            return result | std::views::transform(T2_modifier);
        }
        else {
            return result | std::views::transform(T3_modifier);
        }
    }
};

#endif
