#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

namespace fsm {
class Fsm;
}

class fsm::Fsm {
    std::vector<int> marks;
    std::vector<char> alphabet;
    std::vector<std::vector<std::vector<int>>> transitions;
    std::set<int> startingStates;
    std::set<int> finalStates;

public:
    Fsm(const std::vector<int> &q, const std::vector<char> &a);
    Fsm(const std::vector<int> &q, const std::vector<char> &a, std::set<int> s, std::set<int> f);
    Fsm(const std::vector<int> &q, const std::vector<char> &a, const std::vector<std::vector<std::vector<int>>> &t);
    Fsm(const std::vector<int> &q, const std::vector<char> &a, const std::vector<std::vector<std::vector<int>>> &t, std::set<int> s, std::set<int> f);

    Fsm(const Fsm &fsm);
    Fsm(Fsm &&fsm);

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
