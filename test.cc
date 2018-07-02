#include "auto.hh"
#include <iostream>

// template<class ...States>
// struct ExInterface;
// template<class ...States>
// struct StateA;
// template<class ...States>
// struct StateB;



template<template <class...> class ...States>
struct ExInterface {
    virtual void callback(Auto<ExInterface, States...> &a) = 0;

    virtual ~ExInterface() {
    }
};


template<template<class...> class ...States>
struct StateA : ExInterface<States...> {
    virtual void callback(Auto<ExInterface, States...> &a) override {
        std::cout << "1" << std::endl;
    }
};


template<template <template<class...> class ...States>
struct StateB : ExInterface<States...> {
    virtual void callback(Auto<ExInterface, States...> &a) override {
        std::cout << "B, entering 1" << std::endl;
        a.template enter<StateA>();
    }
};


using MyAuto = Auto<ExInterface, StateA, StateB>;

int main() {
    MyAuto a;
    a.template initialize<StateA>();
    a.get_state()->callback(a);
    a.get_state()->callback(a);
}
