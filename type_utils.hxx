#pragma once


#include <stddef.h>


template<template<class> class T, template<class> class U>
struct static_ttype_test
{
    static const bool eq = false;
};


template<template<class> class T>
struct static_ttype_test<T, T>
{
    static const bool eq = true;
};

template<class T, class U>
struct static_type_test
{
    static const bool eq = false;
};


template<class T>
struct static_type_test<T, T>
{
    static const bool eq = true;
};

// tests whether two types are equal

template<class T, class U>
constexpr bool static_type_eq()
{
    return static_type_test<T, U>::eq;
}

template<template<class> class T, template<class> class U>
constexpr bool static_type_eq()
{
    return static_ttype_test<T, U>::eq;
}


// counts the number of parameters of the template

template <typename>
constexpr size_t type_count()
{
    return 1;
}

template <typename cur_type, typename next_type, typename ...types>
constexpr size_t type_count()
{
    return 1 + type_count<next_type, types...>();
}


// returns the index of a reference type among a list of other types

template <typename ref_type, typename cur_type, typename ...args>
constexpr size_t type_index()
{
    if (static_type_eq<ref_type, cur_type>())
        return 0;
    return 1 + type_index<ref_type, args...>();
}

template <typename ref_type, typename cur_type>
constexpr size_t type_index()
{
    // a static assert should be used here, as the last type of the list,
    // if reached, must be the one we're looking for
    return 0;
}

template<class ...Elems>
struct TList;

template<template<class> class ...Elems>
struct TTList;

#define MYTEMPL
#define TNAME TList
#include "tlist.hxx"
#undef TNAME
#undef MYTEMPL

#define MYTEMPL template<class>
#define TNAME TTList
#include "tlist.hxx"
#undef TNAME
#undef MYTEMPL

template<class T>
struct SizeofReducer {
    constexpr auto operator()() {
        return sizeof(T);
    }

    template<class NT>
    constexpr auto feed() {
        if constexpr (sizeof(NT) > sizeof(T))
            return SizeofReducer<NT>{};
        else
            return SizeofReducer<T>{};
    }
};

template<class T>
struct AlignofReducer {
    constexpr auto operator()() {
        return alignof(T);
    }

    template<class NT>
    constexpr auto feed() {
        if constexpr (alignof(NT) > alignof(T))
            return AlignofReducer<NT>{};
        else
            return AlignofReducer<T>{};
    }
};

template<template<class> class OldState, template<class> class NewState>
struct TTPair {
    template<class T> using old_t = OldState<T>;
    template<class T> using new_t = NewState<T>;
};
