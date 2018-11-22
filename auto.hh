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

template<class Auto>
struct NoopAutoLogger {
    template<template<class> class NewState,
             template<class> class OldState>
    void log() {
    }
};


template<template<class> class PInterface,
         class AllowedTransitions,
         class States,
	 template<class> class Logger = NoopAutoLogger>
class Auto {
    using self_t = Auto<PInterface, AllowedTransitions, States, Logger>;
    using intf_t = PInterface<self_t>;

    template<template <class Proxy> class State>
    struct Proxy {
        using auto_t = Auto<PInterface, AllowedTransitions, States, Logger>;
        using self_t = Proxy<State>;
        using map_t = self_t;
        using state_t = State<self_t>;
        using intf_t = typename Auto::intf_t;

        template<template<class> class DestState, class ...Args>
        static void transit(Auto &a, Args&& ...args) {
            a.free_transit<DestState, State, Args...>(std::forward<Args>(args)...);
        }
    };

    using StatesProxies = decltype(States::template Tmap<Proxy>());

    template<class Proxy>
    struct ProxyState {
        using map_t = typename Proxy::state_t;
    };
    using ProxiedStates = decltype(StatesProxies::template Tmap<ProxyState>());

    struct StateData {
        alignas(ProxiedStates::template reduce<AlignofReducer>()())
        uint8_t state_data_[ProxiedStates::template reduce<SizeofReducer>()()];

        template<class T>
        T* get_data() {
            T * volatile res = static_cast<T*>(static_cast<void*>(state_data_));
            return /*std::launder(*/res/*)*/;
        }

        template<template <class> class State, class ...Args>
        auto enter(Args&& ...args) {
            // TODO: move this assert at class construction time
            static_assert(std::is_base_of<intf_t, typename Proxy<State>::state_t>::value);
            static_assert(StatesProxies::template contains<Proxy<State>>());
            using tstate = typename Proxy<State>::state_t;
            return new (&state_data_) tstate(std::forward<Args...>(args)...);
        }

        void leave() {
            // The destructor must be virtual
            get_data<intf_t>()->~intf_t();
        }
    };

    // there are two banks of state memory.
    // The second one is needed to safely build a new state
    // while having the former's memory still valid
    StateData data_[2];

    // the current's state index
    bool data_pos_;

    Logger<self_t> logger;

    StateData &cur_data() {
        return data_[data_pos_];
    }

    StateData &other_data() {
        return data_[!data_pos_];
    }

    // these aren't public to avoid multiple initializations
    // constructors cannot be templated, so the initial state
    // parameters can't be given here
    Auto() : data_pos_(0) {}

    Auto(Logger<self_t> &&log)
      : data_pos_(0), logger(log) {}

    // internal for switching to a state
    template<template<class> class NewState, class ...Args>
    void enter(Args&& ...args) {
        other_data().template enter<NewState, Args...>(
            std::forward<Args>(args)...);
        cur_data().leave();
        data_pos_ = !data_pos_;
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

        logger.template log<NewState, OldState>();

        enter<NewState, Args...>(std::forward<Args>(args)...);
    }
public:
    ~Auto() {
        cur_data().leave();
    }

    // constructors cannot be templated, so init() will do
    template<template<class> class InitialState, class ...Args>
    static Auto init(Args&& ...args) {
        Auto res;
        res.cur_data().template enter<InitialState, Args...>(
            std::forward<Args>(args)...);
	return res;
    }

    // init with a logger.
    template<template<class> class InitialState, class ...Args>
    static Auto init_logger(Logger<self_t> &&logger, Args&& ...args) {
        auto res = Auto{std::move(logger)};
        res.cur_data().template enter<InitialState, Args...>(
            std::forward<Args>(args)...);
	return res;
    }

    auto &get_state() {
        return *cur_data().template get_data<intf_t>();
    }

    template<template<class> class State>
    static std::string state_name() {
        return typeid(typename Proxy<State>::state_t).name();
    }
};
