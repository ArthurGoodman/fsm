#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

#include "matrix.h"

namespace fsm {
template <class>
class Fsm;
}

template <class S>
class fsm::Fsm {
    std::vector<S> states;
    std::vector<char> alphabet;
    Matrix<std::set<int>> transitions;
    std::set<int> startingStates;
    std::set<int> finalStates;

public:
    Fsm(const std::vector<S> &states, const std::vector<char> &alphabet);
    Fsm(const std::vector<S> &states, const std::vector<char> &alphabet, std::set<int> startingStates, std::set<int> finalStates);
    Fsm(const std::vector<S> &states, const std::vector<char> &alphabet, const std::vector<std::vector<std::set<int>>> &transitions);
    Fsm(const std::vector<S> &states, const std::vector<char> &alphabet, const std::vector<std::vector<std::set<int>>> &transitions, std::set<int> startingStates, std::set<int> finalStates);

    Fsm(const Fsm<S> &fsm);
    Fsm(Fsm<S> &&fsm);

    void connect(int s1, int s2, char a);
    void connect(int s1, int s2, int a);
    void start(int startingStates);
    void finish(int startingStates);

    void inspect() const;

    Fsm rev() const;
    Fsm det() const;
    Fsm min() const;

private:
    void printState(int state) const;
    std::vector<std::set<int>> closure(std::vector<std::set<int>> &q, int state) const;
};

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &states, const std::vector<char> &alphabet)
    : states(states), alphabet(alphabet), transitions(alphabet.size(), states.size()) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &states, const std::vector<char> &alphabet, std::set<int> startingStates, std::set<int> finalStates)
    : states(states), alphabet(alphabet), transitions(alphabet.size(), states.size()), startingStates(startingStates), finalStates(finalStates) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &states, const std::vector<char> &alphabet, const std::vector<std::vector<std::set<int>>> &transitions)
    : states(states), alphabet(alphabet), transitions(transitions) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &states, const std::vector<char> &alphabet, const std::vector<std::vector<std::set<int>>> &transitions, std::set<int> startingStates, std::set<int> finalStates)
    : states(states), alphabet(alphabet), transitions(transitions), startingStates(startingStates), finalStates(finalStates) {
}

template <class S>
fsm::Fsm<S>::Fsm(const Fsm<S> &fsm)
    : states(fsm.states), alphabet(fsm.alphabet), transitions(fsm.transitions), startingStates(fsm.startingStates), finalStates(fsm.finalStates) {
}

template <class S>
fsm::Fsm<S>::Fsm(Fsm<S> &&fsm)
    : states(std::move(fsm.states)), alphabet(std::move(fsm.alphabet)), transitions(std::move(fsm.transitions)), startingStates(std::move(fsm.startingStates)), finalStates(std::move(fsm.finalStates)) {
}

template <class S>
void fsm::Fsm<S>::connect(int s1, int s2, char a) {
    transitions[s1][std::find(alphabet.begin(), alphabet.end(), a) - alphabet.begin()].insert(s2);
}

template <class S>
void fsm::Fsm<S>::connect(int s1, int s2, int a) {
    transitions[s1][a].insert(s2);
}

template <class S>
void fsm::Fsm<S>::start(int state) {
    startingStates.insert(state);
}

template <class S>
void fsm::Fsm<S>::finish(int state) {
    finalStates.insert(state);
}

template <class S>
void fsm::Fsm<S>::inspect() const {
    for (int s1 = 0; s1 < (int)states.size(); s1++)
        for (int c = 0; c < (int)alphabet.size(); c++)
            for (int s2 : transitions.at(s1, c)) {
                printState(s1);
                std::cout << " --" << alphabet[c] << "-> ";
                printState(s2);
                std::cout << "\n";
            }
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::rev() const {
    Fsm<S> rfsm(states, alphabet, finalStates, startingStates);

    for (int s1 = 0; s1 < (int)states.size(); s1++)
        for (int a = 0; a < (int)alphabet.size(); a++)
            for (int s2 : transitions.at(s1, a))
                rfsm.connect(s2, s1, a);

    return rfsm;
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::det() const {
    std::vector<std::set<int>> q;

    q.push_back(startingStates);

    std::vector<std::vector<std::set<int>>> newTransitions;

    while (newTransitions.size() < q.size())
        newTransitions.push_back(closure(q, newTransitions.size()));

    std::vector<S> newStates(q.size());
    std::iota(newStates.begin(), newStates.end(), 0);

    std::set<int> newFinalStates;

    for (int i = 0; i < (int)q.size(); i++)
        for (int s : q[i])
            if (finalStates.find(s) != finalStates.end())
                newFinalStates.insert(i);

    return Fsm(newStates, alphabet, newTransitions, {0}, newFinalStates);
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::min() const {
    return rev().det().rev().det();
}

template <class S>
void fsm::Fsm<S>::printState(int state) const {
    std::cout << (startingStates.find(state) != startingStates.end() ? "*" : " ") << states[state] << (finalStates.find(state) != finalStates.end() ? "*" : " ");
}

template <class S>
std::vector<std::set<int>> fsm::Fsm<S>::closure(std::vector<std::set<int>> &q, int state) const {
    std::vector<std::set<int>> t;

    for (int a = 0; a < (int)alphabet.size(); a++) {
        std::set<int> ts;

        for (int i : q[state])
            for (int s : transitions.at(i, a))
                ts.insert(s);

        if (ts.empty()) {
            t.push_back(std::set<int>());
            continue;
        }

        int index;

        std::vector<std::set<int>>::iterator i = std::find(q.begin(), q.end(), ts);

        if (i == q.end()) {
            index = q.size();
            q.push_back(ts);
        } else
            index = i - q.begin();

        t.push_back(std::set<int>({index}));
    }

    return t;
}
