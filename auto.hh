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
class BaseAuto {
protected:
    using base_auto_t = BaseAuto<PInterface, States...>;
    using raw_tlist = TTList<States...>;
    using Interface = PInterface<base_auto_t>;

    template<template<class> class State>
    struct auto_t_mapper {
        using type = State<base_auto_t>;
    };

    using unsafe_tlist = decltype(raw_tlist::template Tmap<auto_t_mapper>());

    struct StateData {
        alignas(unsafe_tlist::template reduce<AlignofReducer>()())
        uint8_t state_data_[unsafe_tlist::template reduce<SizeofReducer>()()];

        template<class T>
        T* get_data() {
            auto res = static_cast<T*>(static_cast<void*>(state_data_));
            return /*std::launder(*/res/*)*/;
        }

        template<template <class> class State, class ...Args>
        auto enter(Args&& ...args) {
            // TODO: move this assert at class construction time
            static_assert(std::is_base_of<Interface, State<base_auto_t>>::value);
            static_assert(map_pack_contains<base_auto_t, State, States...>());
            return new (&state_data_) State<base_auto_t>(std::forward(args)...);
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

    template<template<class> class NewState, class ...Args>
    void enter(Args&& ...args) {
        other_data().template enter<NewState, args...>(
            std::forward<Args>(args)...);
        cur_data().leave();
        data_pos_ = !data_pos_;
    }

public:
    auto &get_state() {
        return *cur_data().template get_data<Interface>();
    }

    BaseAuto() : data_pos_(0) {}

    ~BaseAuto() {
        cur_data().leave();
    }

    template<template<class> class InitialState, class ...Args>
    void initialize(Args&& ...args) {
        cur_data().template enter<InitialState, args...>(std::forward<Args>(args)...);
    }
};

template<template<class> class PInterface,
         template<class> class ...States>
struct Auto : public BaseAuto<PInterface, States...> {
    using base_auto_t = BaseAuto<PInterface, States...>;
    using auto_t = Auto<PInterface, States...>;

    template<template<class> class State>
    struct AutoProxy : base_auto_t {
        template<template<class> class NewState, class ...Args>
        void enter(Args&& ...args) {
            static_assert(auto_t::template check_transition<State, NewState>());
            base_auto_t::template enter<NewState, Args...>(
                std::forward<Args>(args)...);
        }
    };

    template<template<class> class State>
    struct StateMapper {
        using type = State<auto_t::AutoProxy<State>>;
    };

    using tlist = decltype(base_auto_t::raw_tlist::template Tmap<StateMapper>());

    template<template<class> class Source, template<class> class Dest>
    static bool constexpr check_transition() {
        return true;
    }

};
