#pragma once

#include <new>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

#include "type_utils.hxx"

template<template<class> class PInterface,
         template<class> class ...States>
class Auto {
    using self_t = Auto<PInterface, States...>;
    using Interface = PInterface<self_t>;

    struct StateData {
        alignas(max_map_pack<self_t, AlignofMap, States...>())
        uint8_t state_data_[max_map_pack<self_t, SizeofMap, States...>()];

        template<class T>
        T* get_data() {
            auto res = static_cast<T*>(static_cast<void*>(state_data_));
            return /*std::launder(*/res/*)*/;
        }

        template<template <class> class State, class ...Args>
        auto enter(Args&& ...args) {
            // TODO: move this assert at class construction time
            static_assert(std::is_base_of<Interface, State<self_t>>::value);
            static_assert(map_pack_contains<self_t, State, States...>());
            return new (&state_data_) State<self_t>(std::forward(args)...);
        }

        void leave() {
            // The destructor must be virtual
            get_data<Interface>()->~Interface();
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


public:

    auto &get_state() {
        return *cur_data().template get_data<Interface>();
    }

    Auto() : data_pos_(0) {}

    ~Auto() {
        cur_data().leave();
    }

    template<template<class> class InitialState, class ...Args>
    void initialize(Args&& ...args) {
        cur_data().template enter<InitialState, args...>(std::forward<Args>(args)...);
    }

    template<template<class> class NewState, class ...Args>
    void enter(Args&& ...args) {
        other_data().template enter<NewState, args...>(std::forward<Args>(args)...);
        cur_data().leave();
        data_pos_ = !data_pos_;
    }
};
