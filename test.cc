#include "auto.hh"
#include <iostream>

// template<class ...States>
// struct ExInterface;
// template<class ...States>
// struct StateA;
// template<class ...States>
// struct StateB;



template<class Auto>
struct ExInterface {
    virtual void callback(Auto &a) = 0;

    virtual ~ExInterface() {
    }
};


template<class Auto>
struct StateA : ExInterface<Auto::common> {
    virtual void callback(Auto &a) override {
        std::cout << "1" << std::endl;
    }
};


template<class Auto>
struct StateB : ExInterface<Auto::common> {
    virtual void callback(Auto &a) override {
        std::cout << "B, entering 1" << std::endl;
        a.template enter<StateA>();
    }
};


using MyAuto = Auto<ExInterface, StateA, StateB>;

int main() {
    MyAuto a;
    a.template initialize<StateB>();
    a.get_state().callback(a);
    a.get_state().callback(a);
}
