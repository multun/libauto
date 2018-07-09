#include <cstddef>

#define MYCLASS MYTEMPL class

template<MYCLASS ...Elems>
struct TNAME {
    using type = TNAME<Elems...>;

    template<template<MYCLASS...> class Templ, MYCLASS ...Args>
    struct TParam {
        using type = Templ<Args...>;
    };

    template<template<MYCLASS...> class Templ>
    static constexpr auto apply() {
        return typename TParam<Templ, Elems...>::type{};
    }

    template<MYCLASS T>
    static constexpr auto push_front() {
        return TNAME<T, Elems...>{};
    }

    // unit helps working around syntaxical limitations
    // where templated types need guiding. Good luck writting the stop
    // case of the map function without a similar trick
    template<MYCLASS T>
    static constexpr auto unit() {
        return TNAME<T>{};
    }

    template<MYCLASS T>
    static constexpr auto push_back() {
        return TNAME<Elems..., T>{};
    }

    template<template <MYCLASS> class F>
    struct mapper {
        template<MYCLASS First, MYCLASS Second, MYCLASS ...More>
        static constexpr auto map() {
            auto tail = mapper<F>::template map<Second, More...>();
            return tail.template push_front<F<First>::type>();
        }

        template<MYCLASS Elem>
        static constexpr auto map() {
            return unit<F<Elem>::type>();
        }
    };

    template<template <MYCLASS> class F>
    static constexpr auto map() {
        return mapper<F>::template map<Elems...>();
    }

    template<template<MYCLASS> class Reducer>
    struct reducer {
        template<MYCLASS First, MYCLASS Second, MYCLASS ...More>
        static constexpr auto reduce() {
            auto tail = reducer<Reducer>::template reduce<Second, More...>();
            return tail.template feed<First>();
        }

        template<MYCLASS Elem>
        static constexpr auto reduce() {
            return Reducer<Elem>{};
        }
    };

    template<template<MYCLASS> class Reducer>
    static constexpr auto reduce() {
        return reducer<Reducer>::template reduce<Elems...>();
    }
};
