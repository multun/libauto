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


using trans = TList<TPair<StateB, StateA>>;
using MyAuto = Auto<ExInterface, trans, StateA, StateB>;

int main() {
    auto a = MyAuto::template init<StateB>(1);
    a.get_state().callback(a);
    a.get_state().callback(a);
}
