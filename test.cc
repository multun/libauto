#include "auto.hh"
#include <iostream>

template<class Auto>
struct ExInterface {
    virtual void callback(Auto &a) = 0;

    virtual ~ExInterface() {
    }
};


template<class Auto>
struct StateA : ExInterface<Auto> {
    const std::string message_;

    StateA(std::string &&message) : message_{message} {
    }

    virtual void callback(Auto &) override {
        std::cout << message_ << std::endl;
    }

    ~StateA() {
        std::cout << "destroying state a" << std::endl;
    }
};


template<class Auto>
struct StateB : ExInterface<Auto> {
    int i;
    StateB(int i_):i{i_}{
    }

    virtual void callback(Auto &a) override {
        std::cout << "B, entering 1" << std::endl;
        a.template transit<StateA>(this, "test");
    }

    ~StateB() {
        std::cout << "destroying state b" << std::endl;
    }
};


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
using States = TTList<StateA, StateB>;
using MyAuto = Auto<ExInterface, Transitions, States>;
using MyLoggingAuto = Auto<ExInterface, Transitions, States, OverkillLogger>;

void logging_example() {
    auto logger = OverkillLogger<MyLoggingAuto>{42};
    auto a = MyLoggingAuto::template init<StateB>(std::move(logger), 1);
    a.get_state().callback(a);
    a.get_state().callback(a);
}

void silent_example() {
    auto a = MyAuto::template init<StateB>(1);
    a.get_state().callback(a);
    a.get_state().callback(a);
}

int main() {
    logging_example();
    silent_example();
}
