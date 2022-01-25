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

template <typename T, modifier<T> F1, modifier<T> F2>
modifier<T> auto compose (F1 f1, F2 f2) {
    return [=](T t) {
        return f1(f2(t));
    };
}

template <typename T1, typename T2, typename T3>
class tri_list {
    using variant = std::variant<T1, T2, T3>;

    std::vector<variant> list;

    std::function<T1(T1)> T1_modifier = identity<T1>;
    std::function<T2(T2)> T2_modifier = identity<T2>;
    std::function<T3(T3)> T3_modifier = identity<T3>;

public:

    tri_list() = default;

    tri_list(std::initializer_list<variant> list) : list(list) {}

    template<class T>
    auto range_over() {
        auto result = list
            | std::views::filter([](variant element) {
                return std::holds_alternative<T>(element);
            })
            | std::views::transform([](variant element) {
                return std::get<T>(element);
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

    template <typename T>
    void push_back(const T& t) {
        list.push_back(t);
    }

    template <typename T>
    void reset() {

    }

    template <typename T, modifier<T> F>
    void modify_only(F f = F{}) {

    }
};

#endif
