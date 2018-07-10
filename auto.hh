#pragma once

#include <new>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <typeinfo>
#include <iostream>

#include "type_utils.hxx"

template<template<class> class PInterface,
         class AllowedTransitions,
         template<class> class ...States>
class Auto {
    using self_t = Auto<PInterface, AllowedTransitions, States...>;
    using intf_t = PInterface<self_t>;
    using ttlist = TTList<States...>;

    template<template<class> class State>
    struct Automaker {
        using type = State<self_t>;
    };

    using tlist = decltype(ttlist::template Tmap<Automaker>());

    struct StateData {
        alignas(tlist::template reduce<AlignofReducer>()())
        uint8_t state_data_[tlist::template reduce<SizeofReducer>()()];

        template<class T>
        T* get_data() {
            T * volatile res = static_cast<T*>(static_cast<void*>(state_data_));
            return /*std::launder(*/res/*)*/;
        }

        template<template <class> class State, class ...Args>
        auto enter(Args&& ...args) {
            // TODO: move this assert at class construction time
            static_assert(std::is_base_of<intf_t, State<self_t>>::value);
            static_assert(tlist::template contains<State<self_t>>());
            return new (&state_data_) State<self_t>(std::forward<Args...>(args)...);
        }

        void leave() {
            // The destructor must be virtual
            get_data<intf_t>()->~intf_t();
        }
    };

    StateData data_[2];
    bool data_pos_;

    StateData &cur_data() {
        return data_[data_pos_];
    }

    StateData &other_data() {
        return data_[!data_pos_];
    }

    Auto() : data_pos_(0) {}

public:
    auto &get_state() {
        return *cur_data().template get_data<intf_t>();
    }

    ~Auto() {
        cur_data().leave();
    }

    template<template<class> class InitialState, class ...Args>
    static Auto init(Args&& ...args) {
        Auto res;
        res.cur_data().template enter<InitialState, Args...>(
            std::forward<Args>(args)...);
	return res;
    }

    template<template<class> class NewState, class ...Args>
    void enter(Args&& ...args) {
        other_data().template enter<NewState, Args...>(
            std::forward<Args>(args)...);
        cur_data().leave();
        data_pos_ = !data_pos_;
    }

    template<template<class> class NewState,
             template<class> class OldState,
             class Auto>
    static void default_logger(Auto) {
        std::clog << "transition from " << typeid(OldState<Auto>).name() << " to "
	          << typeid(NewState<Auto>).name() << std::endl;
    }

    template<template<class> class NewState,
             template<class> class OldState,
             class ...Args>
    void free_transit(Args&& ...args) {
        static_assert(
            AllowedTransitions::template contains<TTPair<OldState, NewState>>(),
            "forbidden transition");


        /* traits on variadic templates would be mandatory,
           which is too annoying to be worth it */

#ifdef DEFAULT_LOG_TRANSITIONS
# define LOG_TRANSITIONS default_logger
#endif

#ifdef LOG_TRANSITIONS
	LOG_TRANSITIONS<NewState, OldState>(this);
#endif

        enter<NewState, Args...>(std::forward<Args>(args)...);
    }

    template<template<class> class NewState,
             template<class> class OldState,
             class ...Args>
    void transit(OldState<self_t> *, Args&& ...args) {
        free_transit<NewState, OldState, Args...>(std::forward<Args>(args)...);
    }
};
