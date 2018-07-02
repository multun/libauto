#pragma once

#include <new>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

template<template<template<class ...> class ...> class Interface,
         template<class ...> class ...States>
class Auto {
    struct StateData {
        /* alignas(States...) */
        // uint8_t state_data_[sizeof...(States)];
        uint8_t state_data_[1000];

        template<class T>
        T* get_data() {
            return /*std::launder(*/static_cast<T*>(&state_data_)/*)*/;
        }

        template<template <template<class ...> class ...> class State, class ...Args>
        auto enter(Args&& ...args) {
            static_assert(std::is_base_of<
                          Interface<States...>,
                          State<States...>>::value);
            return new (&state_data_) State<States...>(
                std::forward<Args>(args)...);
        }

        void leave() {
            // The destructor must be virtual
            get_data<Interface<States...>>()->~Interface();
        }
    };

    StateData data_[2];
    bool data_pos_;

    ~Auto() {
        cur_data().leave();
    }

    StateData &cur_data() {
        return data_[data_pos_];
    }

    StateData &other_data() {
        return data_[!data_pos_];
    }

    auto &get_state() {
        using IPack = Interface<States...>;
        return *cur_data().template get_data<IPack>();
    }


public:
    Auto() : data_pos_(0) {}

    template<class InitialState, class ...Args>
    void initialize(Args&& ...args) {
        // state_ = type_index<InitialState, States...>();
        cur_data().template enter<InitialState, args...>(std::forward<Args>(args)...);
    }

    template<class NewState, class ...Args>
    void enter(Args&& ...args) {
        // state_ = type_index<NewState, States...>();
        other_data().template enter<NewState, args...>(std::forward<Args>(args)...);
        get_state().leave();
        data_pos_ = !data_pos_;
    }
};
