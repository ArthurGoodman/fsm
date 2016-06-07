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
    Fsm(const std::vector<S> &q, const std::vector<char> &a);
    Fsm(const std::vector<S> &q, const std::vector<char> &a, std::set<int> s, std::set<int> f);
    Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t);
    Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t, std::set<int> s, std::set<int> f);

    Fsm(const Fsm<S> &fsm);
    Fsm(Fsm<S> &&fsm);

    void connect(int s1, int s2, char a);
    void connect(int s1, int s2, int a);
    void start(int state);
    void finish(int state);

    void inspect() const;

    Fsm rev() const;
    Fsm det() const;
    Fsm min() const;

private:
    void printState(int state) const;
    std::vector<std::set<int>> epsilonClosures() const;
    void epsilonClosure(int state, std::vector<std::set<int>> &ec, std::vector<bool> &flags) const;
};

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a)
    : states(q), alphabet(a), transitions(a.size() + 1, q.size()) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a, std::set<int> s, std::set<int> f)
    : states(q), alphabet(a), transitions(a.size() + 1, q.size()), startingStates(s), finalStates(f) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t)
    : states(q), alphabet(a), transitions(t) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t, std::set<int> s, std::set<int> f)
    : states(q), alphabet(a), transitions(t), startingStates(s), finalStates(f) {
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
    transitions[s1][a == '\0' ? alphabet.size() : std::find(alphabet.begin(), alphabet.end(), a) - alphabet.begin()].insert(s2);
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
        for (int c = 0; c < (int)alphabet.size() + 1; c++)
            for (int s2 : transitions.at(s1, c)) {
                printState(s1);

                if (c == (int)alphabet.size())
                    std::cout << " --->> ";
                else
                    std::cout << " --" << alphabet[c] << "-> ";

                printState(s2);

                std::cout << "\n";
            }
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::rev() const {
    Fsm<S> rfsm(states, alphabet, finalStates, startingStates);

    for (int s1 = 0; s1 < (int)states.size(); s1++)
        for (int a = 0; a < (int)alphabet.size() + 1; a++)
            for (int s2 : transitions.at(s1, a))
                rfsm.connect(s2, s1, a);

    return rfsm;
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::det() const {
    std::vector<std::set<int>> ec = epsilonClosures();

    std::vector<std::set<int>> q;

    std::set<int> q0;

    for (int s : startingStates)
        q0.insert(ec[s].begin(), ec[s].end());

    q.push_back(q0);

    std::vector<std::vector<std::set<int>>> t;

    while (t.size() < q.size()) {
        std::vector<std::set<int>> row;

        for (int a = 0; a < (int)alphabet.size(); a++) {
            std::set<int> ts;

            for (int i : q[t.size()])
                for (int s : transitions.at(i, a))
                    ts.insert(ec[s].begin(), ec[s].end());

            if (ts.empty()) {
                row.push_back({});
                continue;
            }

            int index;

            std::vector<std::set<int>>::iterator i = std::find(q.begin(), q.end(), ts);

            if (i == q.end()) {
                index = q.size();
                q.push_back(ts);
            } else
                index = i - q.begin();

            row.push_back({index});
        }

        row.push_back({});

        t.push_back(row);
    }

    std::vector<S> s(q.size());
    std::iota(s.begin(), s.end(), 0);

    std::set<int> f;

    for (int i = 0; i < (int)q.size(); i++)
        for (int s : q[i])
            if (finalStates.find(s) != finalStates.end())
                f.insert(i);

    return Fsm(s, alphabet, t, {0}, f);
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
std::vector<std::set<int>> fsm::Fsm<S>::epsilonClosures() const {
    std::vector<std::set<int>> ec(states.size());
    std::vector<bool> flags(states.size(), false);

    for (int i = 0; i < (int)ec.size(); i++)
        ec[i].insert(i);

    for (int s = 0; s < (int)states.size(); s++)
        epsilonClosure(s, ec, flags);

    return ec;
}

template <class S>
void fsm::Fsm<S>::epsilonClosure(int state, std::vector<std::set<int>> &ec, std::vector<bool> &flags) const {
    if (flags[state])
        return;

    flags[state] = true;

    for (int s : transitions.at(state, alphabet.size())) {
        ec[state].insert(s);
        epsilonClosure(s, ec, flags);
    }
}
