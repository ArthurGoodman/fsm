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
    std::vector<S> q;
    std::vector<char> a;
    Matrix<std::set<int>> t;
    std::set<int> s;
    std::set<int> f;

public:
    Fsm(const std::vector<S> &q, const std::vector<char> &a);
    Fsm(const std::vector<S> &q, const std::vector<char> &a, std::set<int> s, std::set<int> f);
    Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t);
    Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t, std::set<int> s, std::set<int> f);

    void connect(int s1, int s2, char c);
    void start(int s);
    void finish(int s);

    void inspect() const;

    Fsm rev() const;
    Fsm det() const;
    Fsm min() const;

private:
    void printState(int state) const;
};

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a)
    : q(q), a(a), t(a.size(), q.size()) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a, std::set<int> s, std::set<int> f)
    : q(q), a(a), t(a.size(), q.size()), s(s), f(f) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t)
    : q(q), a(a), t(t) {
}

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t, std::set<int> s, std::set<int> f)
    : q(q), a(a), t(t), s(s), f(f) {
}

template <class S>
void fsm::Fsm<S>::connect(int s1, int s2, char c) {
    t[s1][std::find(a.begin(), a.end(), c) - a.begin()].insert(s2);
}

template <class S>
void fsm::Fsm<S>::start(int state) {
    s.insert(state);
}

template <class S>
void fsm::Fsm<S>::finish(int state) {
    f.insert(state);
}

template <class S>
void fsm::Fsm<S>::inspect() const {
    for (unsigned s1 = 0; s1 < q.size(); s1++)
        for (unsigned c = 0; c < a.size(); c++)
            for (int s2 : t.at(s1, c)) {
                printState(s1);
                std::cout << " --" << a[c] << "-> ";
                printState(s2);
                std::cout << "\n";
            }
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::rev() const {
    Fsm<S> rfsm(q, a, f, s);

    for (unsigned s1 = 0; s1 < q.size(); s1++)
        for (unsigned c = 0; c < a.size(); c++)
            for (int s2 : t.at(s1, c))
                rfsm.connect(s2, s1, a[c]);

    return rfsm;
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::det() const {
    return *this;
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::min() const {
    return rev().det().rev().det();
}

template <class S>
void fsm::Fsm<S>::printState(int state) const {
    std::cout << (s.find(state) != s.end() ? "*" : " ") << q[state] << (f.find(state) != f.end() ? "*" : " ");
}
