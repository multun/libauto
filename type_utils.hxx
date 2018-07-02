#pragma once


#include <stddef.h>


template<typename T, typename U>
struct static_type_test
{
    static const bool eq = false;
};


template<typename T>
struct static_type_test<T, T>
{
    static const bool eq = true;
};


// tests whether two types are equal

template<typename T, typename U>
constexpr bool static_type_eq()
{
    return static_type_test<T, U>::eq;
}


// counts the number of parameters of the template

template <typename cur_type, typename ...args>
constexpr size_t type_count()
{
    return 1 + type_count<args...>();
}

template <typename>
constexpr size_t type_count()
{
    return 1;
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
