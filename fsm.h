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
    Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t);
    Fsm(const std::vector<S> &q, const std::vector<char> &a, const std::vector<std::vector<std::set<int>>> &t, std::set<int> s, std::set<int> f);

    void connect(int s1, int s2, char c);
    void start(int s);
    void finish(int s);

    void inspect() const;

    Fsm rev() const;
    Fsm det() const;
    Fsm min() const;
};

template <class S>
fsm::Fsm<S>::Fsm(const std::vector<S> &q, const std::vector<char> &a)
    : q(q), a(a), t(a.size(), q.size()) {
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
    for (unsigned int s1 = 0; s1 < q.size(); s1++)
        for (unsigned int c = 0; c < a.size(); c++)
            for (int s2 : t.at(s1, c))
                std::cout << (s.find(s1) != s.end() ? "* " : "  ") << q[s1] << " --" << a[c] << "-> " << q[s2] << (f.find(s2) != f.end() ? " *\n" : "\n");
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::rev() const {
    return *this;
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::det() const {
    return *this;
}

template <class S>
fsm::Fsm<S> fsm::Fsm<S>::min() const {
    return rev().det().rev().det();
}
