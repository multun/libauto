#include "auto.hh"
#include <iostream>

// interface all the states of the automaton have to inherit from
template<class Auto>
struct ExInterface {
    virtual void callback(Auto &a) = 0;

    virtual ~ExInterface() {
    }
};

template<class Auto>
struct StateA : ExInterface<Auto> {
    // no need to be trivially constructible
    const std::string message_;

    StateA(std::string &&message)
        : message_{message} {
        std::cout << "[StateA] initializing with message=" << message << std::endl;
    }

    virtual void callback(Auto &) override {
        std::cout << "[StateA][callback] " << message_ << std::endl;
    }

    ~StateA() {
        std::cout << "[StateA] destroying" << std::endl;
    }
};

template<class Auto>
struct StateB : ExInterface<Auto> {
    int i_;

    StateB(int i)
        : i_{i} {
        std::cout << "[StateB] initializing with i=" << i << std::endl;
    }

    virtual void callback(Auto &a) override {
        std::cout << "[StateB][callback] transition to StateA" << std::endl;
        a.template transit<StateA>(this, "test");
    }

    ~StateB() {
        std::cout << "[StateB] destroying" << std::endl;
    }
};

// loggers my be stateful
template<class Auto>
struct OverkillLogger {
    size_t log_count = 0;

    OverkillLogger(size_t msg_id)
        : log_count{msg_id} {}

    template<template<class> class NewState,
             template<class> class OldState>
    void log() {
        std::clog << log_count++ << " transition from "
                  << typeid(OldState<Auto>).name() << " to "
	          << typeid(NewState<Auto>).name() << std::endl;
    }
};

using Transitions = TList<TTPair<StateB, StateA>>;
// packing states into a further abstraction enables reusing
// the list / having optional template parameters
using States = TTList<StateA, StateB>;

void logging_example() {
    using MyLoggingAuto = Auto<ExInterface, Transitions, States, OverkillLogger>;
    auto logger = OverkillLogger<MyLoggingAuto>{42};
    auto a = MyLoggingAuto::template init<StateB>(std::move(logger), 1);
    a.get_state().callback(a);
    a.get_state().callback(a);
}

void silent_example() {
    using MyAuto = Auto<ExInterface, Transitions, States>;
    auto a = MyAuto::template init<StateB>(1);
    a.get_state().callback(a);
    a.get_state().callback(a);
}

int main() {
    logging_example();
    silent_example();
}
