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


template <class T>
struct AlignofMap {
    constexpr size_t operator()() {
        return alignof(T);
    }
};

template <class T>
struct SizeofMap {
    constexpr size_t operator()() {
        return sizeof(T);
    }
};


// not really elegant, could be replaced by a map / reduce
// on a parameter pack object

template <class TArg, template <class> class f,
          template<class> class Arg>
constexpr auto max_map_pack() {
    return f<Arg<TArg>>()();
}

template <class TArg, template <class> class f,
          template<class> class Arg,
          template<class> class NArg,
          template<class> class ...Args>
constexpr auto max_map_pack() {
    auto prev = max_map_pack<TArg, f, NArg, Args...>();
    auto cur = f<Arg<TArg>>()();
    if (cur > prev)
        return cur;
    return prev;
}


template <class TParm,
          template<class> class Elem,
          template<class> class HList>
constexpr bool map_pack_contains() {
    return static_type_eq<Elem<TParm>, HList<TParm>>();
}

template <class TParm,
          template<class> class Elem,
          template<class> class HList,
          template<class> class HHList,
          template<class> class ...Rest>
constexpr bool map_pack_contains() {
    if (static_type_eq<Elem<TParm>, HList<TParm>>())
        return true;
    else
        return map_pack_contains<TParm, Elem, HHList, Rest...>();
}


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
