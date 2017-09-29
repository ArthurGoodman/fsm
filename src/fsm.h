#pragma once

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

namespace fsm {

class Fsm {
    std::vector<char> alphabet;
    std::vector<std::vector<std::vector<int>>> transitions;
    std::set<int> startingStates;
    std::set<int> finalStates;

public:
    Fsm(int states, const std::vector<char> &a);
    Fsm(int states, const std::vector<char> &a, std::set<int> s, std::set<int> f);
    Fsm(const std::vector<char> &a, const std::vector<std::vector<std::vector<int>>> &t);

    Fsm(const std::vector<char> &a,
        const std::vector<std::vector<std::vector<int>>> &t,
        std::set<int> s,
        std::set<int> f);

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

} // namespace fsm
