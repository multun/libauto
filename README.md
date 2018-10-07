# Introduction

A library for making state machines :
 - no dynamic allocation needed
 - all states will share a common interface, which may then be used to interact
 with states
 - transitionning between states is a safe operation (memory owne by the former
 state is still valid when constructing the new one)
 - a list of allowed transitions can be used to restrict what state changes are
 allowed
 - one may log state transitions
 - states are reusable

# Usage

See `test.cc`

# Limitations

 - When transitionning from a state to another, a deduction guide must be given
 for the checker to know from which state the transition comes from. It may be
 "fixed" by introducing a state identifier, which is super easy but adds a
 performance overhead for little apparent benefit. Introducing a proxy class
 which has friendly access for proxying transitions to the automaton sort of
 solves the problem, but makes the syntax heavier.
 - transitionning to a new state while already constructing a state is undefined
 behavior

# FAQ

`is it production ready ?`
No, it's a toy, the almost untested™ kind

`Where's the build system ?`
That's like two header files, what would you like to build ?
