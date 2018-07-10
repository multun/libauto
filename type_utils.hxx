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
