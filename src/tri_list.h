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
#include <memory>
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
    using variant_t = std::variant<T1, T2, T3>;
    using list_t = std::vector<variant_t>;

    using fun_T1_t = std::function<T1(T1)>;
    using fun_T2_t = std::function<T2(T2)>;
    using fun_T3_t = std::function<T3(T3)>;

    using fun_T1_ptr = std::shared_ptr<fun_T1_t>;
    using fun_T2_ptr = std::shared_ptr<fun_T2_t>;
    using fun_T3_ptr = std::shared_ptr<fun_T3_t>;

    list_t list;

    fun_T1_ptr T1_modifier_ptr = std::make_shared<fun_T1_t>(identity<T1>);
    fun_T2_ptr T2_modifier_ptr = std::make_shared<fun_T2_t>(identity<T2>);
    fun_T3_ptr T3_modifier_ptr = std::make_shared<fun_T3_t>(identity<T3>);

    class tri_iterator {
        typename std::vector<variant_t>::iterator it;

        fun_T1_ptr T1_modifier_ptr;
        fun_T2_ptr T2_modifier_ptr;
        fun_T3_ptr T3_modifier_ptr;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::iter_difference_t<typename list_t::iterator>;
        using value_type = variant_t;
        using reference = const variant_t &;
        using pointer = variant_t *;

        tri_iterator() noexcept = default;
        /*explicit */
        tri_iterator(const typename list_t::iterator &i, const fun_T1_ptr &T1_ptr, const fun_T2_ptr &T2_ptr,
                     const fun_T3_ptr &T3_ptr) : it(i), T1_modifier_ptr(T1_ptr), T2_modifier_ptr(T2_ptr),
                     T3_modifier_ptr(T3_ptr) {}

//        children_iterator(const children_iterator &other) noexcept : it(other.it) {}

        bool operator==(const tri_iterator &other) const noexcept {
            return (this->it == other.it);
        }

        bool operator!=(const tri_iterator &other) const noexcept {
            return !(*this == other);
        }

        value_type operator*() const noexcept {
            if (std::holds_alternative<T1>(*it)) {
                return variant_t((*T1_modifier_ptr)(std::get<T1>(*it)));
            }
            else if (std::holds_alternative<T2>(*it)) {
                return variant_t((*T2_modifier_ptr)(std::get<T2>(*it)));
            }
            else {
                return variant_t((*T3_modifier_ptr)(std::get<T3>(*it)));
            }
        }

//        pointer operator->() noexcept {
//            return &(*it);
//        }

        tri_iterator &operator++() noexcept {
            it++;
            return *this;
        }

        tri_iterator &operator--() noexcept {
            it--;
            return *this;
        }

        tri_iterator operator++(int) noexcept {
            tri_iterator temp = *this;
            it++;
            return temp;
        }

        tri_iterator operator--(int) noexcept {
            tri_iterator temp = *this;
            it--;
            return temp;
        }
    };

    template<typename T>
    void check_Ts_not_same() {
        int constexpr cnt = std::same_as<T, T1> + std::same_as<T, T2> + std::same_as<T, T3>;
        static_assert(cnt == 1);
    }

public:

    tri_list() = default;

    tri_list(std::initializer_list<variant_t> list) : list(list) {}

    template<class T>
    auto range_over() {
        check_Ts_not_same<T>();
        auto result = list
            | std::views::filter([](variant_t element) {
                return std::holds_alternative<T>(element);
            })
            | std::views::transform([](variant_t element) {
                return std::get<T>(element);
            });

        if constexpr (std::is_same<T, T1>::value) {
            return result | std::views::transform(*T1_modifier_ptr);
        }
        else if constexpr (std::is_same<T, T2>::value) {
            return result | std::views::transform(*T2_modifier_ptr);
        }
        else {
            return result | std::views::transform(*T3_modifier_ptr);
        }
    }

    template <typename T>
    void push_back(const T& t) {
        check_Ts_not_same<T>();
        list.push_back(t);
    }

    template <typename T>
    void reset() {
        check_Ts_not_same<T>();
        if (std::is_same_v<T, T1>) {
            *T1_modifier_ptr = identity<T1>;
        }
        else if (std::is_same_v<T, T2>) {
            *T2_modifier_ptr = identity<T2>;
        }
        else if (std::is_same_v<T, T3>) {
            *T3_modifier_ptr = identity<T3>;
        }
    }

    template <typename T, modifier<T> F>
    void modify_only(F f = F{}) {
        check_Ts_not_same<T>();
        if constexpr (std::is_same<T, T1>::value) {
            *T1_modifier_ptr = compose<T>(f, *T1_modifier_ptr);
        }
        else if constexpr (std::is_same<T, T2>::value) {
            *T2_modifier_ptr = compose<T>(f, *T2_modifier_ptr);
        }
        else if constexpr (std::is_same<T, T3>::value) {
            *T3_modifier_ptr = compose<T>(f, *T3_modifier_ptr);
        }
    }

    tri_iterator begin() {
        return tri_iterator(list.begin(), T1_modifier_ptr, T2_modifier_ptr, T3_modifier_ptr);
    }

    tri_iterator end() {
        return tri_iterator(list.end(), T1_modifier_ptr, T2_modifier_ptr, T3_modifier_ptr);
    }
};

#endif
