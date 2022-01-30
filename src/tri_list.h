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

    using fun_t1_t = std::function<T1(T1)>;
    using fun_t2_t = std::function<T2(T2)>;
    using fun_t3_t = std::function<T3(T3)>;

    using fun_t1_ptr = std::shared_ptr<fun_t1_t>;
    using fun_t2_ptr = std::shared_ptr<fun_t2_t>;
    using fun_t3_ptr = std::shared_ptr<fun_t3_t>;

    list_t list;

    fun_t1_ptr t1_modifier_ptr = std::make_shared<fun_t1_t>(identity<T1>);
    fun_t2_ptr t2_modifier_ptr = std::make_shared<fun_t2_t>(identity<T2>);
    fun_t3_ptr t3_modifier_ptr = std::make_shared<fun_t3_t>(identity<T3>);

    class tri_iterator {
        typename std::vector<variant_t>::const_iterator it;

        fun_t1_ptr T1_modifier_ptr;
        fun_t2_ptr T2_modifier_ptr;
        fun_t3_ptr T3_modifier_ptr;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::iter_difference_t<typename list_t::const_iterator>;
        using value_type = variant_t;
        using reference = const variant_t &;
        using pointer = variant_t *;

        tri_iterator() noexcept = default;

        tri_iterator(const typename list_t::const_iterator &i, const fun_t1_ptr &T1_ptr, const fun_t2_ptr &T2_ptr,
                     const fun_t3_ptr &T3_ptr) : it(i), T1_modifier_ptr(T1_ptr), T2_modifier_ptr(T2_ptr),
                                                 T3_modifier_ptr(T3_ptr) {}

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
    void check_Ts_not_same() const {
        int constexpr cnt = std::same_as<T, T1> + std::same_as<T, T2> + std::same_as<T, T3>;
        static_assert(cnt == 1);
    }

public:
    tri_list() = default;

    tri_list(std::initializer_list<variant_t> list) : list(list) {}

    template<class T>
    auto range_over() const {
        check_Ts_not_same<T>();

        auto filtered = list | std::views::filter([](variant_t element) {
            return std::holds_alternative<T>(element);
        });

        auto unpacked = filtered | std::views::transform([](variant_t element) {
            return std::get<T>(element);
        });

        if constexpr (std::is_same_v<T, T1>) {
            return unpacked | std::views::transform(*t1_modifier_ptr);
        }
        else if constexpr (std::is_same_v<T, T2>) {
            return unpacked | std::views::transform(*t2_modifier_ptr);
        }
        else {
            return unpacked | std::views::transform(*t3_modifier_ptr);
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
            *t1_modifier_ptr = identity<T1>;
        }
        else if (std::is_same_v<T, T2>) {
            *t2_modifier_ptr = identity<T2>;
        }
        else if (std::is_same_v<T, T3>) {
            *t3_modifier_ptr = identity<T3>;
        }
    }

    template <typename T, modifier<T> F>
    void modify_only(F f = F{}) {
        check_Ts_not_same<T>();
        if constexpr (std::is_same<T, T1>::value) {
            *t1_modifier_ptr = compose<T>(f, *t1_modifier_ptr);
        }
        else if constexpr (std::is_same<T, T2>::value) {
            *t2_modifier_ptr = compose<T>(f, *t2_modifier_ptr);
        }
        else if constexpr (std::is_same<T, T3>::value) {
            *t3_modifier_ptr = compose<T>(f, *t3_modifier_ptr);
        }
    }

    tri_iterator begin() const {
        return tri_iterator(list.begin(), t1_modifier_ptr, t2_modifier_ptr, t3_modifier_ptr);
    }

    tri_iterator end() const {
        return tri_iterator(list.end(), t1_modifier_ptr, t2_modifier_ptr, t3_modifier_ptr);
    }
};

#endif
