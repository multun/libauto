#include <cstddef>

#define MYCLASS MYTEMPL class

template<MYCLASS ...Elems>
struct TNAME;

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

    template<MYCLASS T>
    static constexpr auto push_back() {
        return TNAME<Elems..., T>{};
    }

    // // maps can only produce TLists, couldn't make it output a TTList
    // template<MYCLASS Key>
    // struct contains_c {
    //     template<MYCLASS First, MYCLASS Second, MYCLASS ...More>
    //     static constexpr auto contains() {
    //         if constexpr (static_type_eq<Key, First>())
    //             return true;
    //         else
    //             return constains_c<Key>::template contains<Second, More...>();
    //     }

    //     template<MYCLASS Elem>
    //     static constexpr auto contains() {
    //         return static_type_eq<Key, Elem>()
    //     }
    // };

    // template<MYCLASS Key>
    // static constexpr auto contains() {
    //     return contains_c<Key>::template contains<Elems...>();
    // }

    // maps can only produce TLists, couldn't make it output a TTList
    template<template <MYCLASS> class F>
    struct Tmapper {
        template<MYCLASS First, MYCLASS Second, MYCLASS ...More>
        static constexpr auto map() {
            auto tail = Tmapper<F>::template map<Second, More...>();
            return tail.template push_front<typename F<First>::type>();
        }

        template<MYCLASS Elem>
        static constexpr auto map() {
            return TList<typename F<Elem>::type>{};
        }
    };

    template<template <MYCLASS> class F>
    static constexpr auto Tmap() {
        return Tmapper<F>::template map<Elems...>();
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
