# Introduction

A library for making state machines :
 - no dynamic allocation needed
 - all states will share a common interface, which may then be used to interact
 with states (using virtual methods)
 - transitionning between states is a safe operation (memory owned by the former
 state is still valid when constructing the new one)
 - a list of allowed transitions can be used to restrict what state changes are
 allowed
 - a hook enables auditing state transitions at runtime
 - states are reusable / pluggable
 - no information about the current state is stored at runtime, everything is
 type powered

# Usage

See `test.cc`

# Limitations

 - When transitionning from a state to another, a proxy class which has friendly
 access to the state machine performs the transition. It it needed to avoid needing
 a deduction guide / forcing the user to explicitly write what's the current state.
 The syntax for transitions is sort of heavy due du C++'s `template` and `typename`
 madness.
 - transitionning to a new state while already constructing a state is undefined
 behavior

# FAQ

`is it production ready ?`  
No, it's a toy, the almost untested™ kind

`Where's the build system ?`  
That's like two header files, what would you like to build ?
