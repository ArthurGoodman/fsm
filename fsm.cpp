#include "fsm.h"

fsm::Fsm::Fsm(int states, const std::vector<char> &a)
    : alphabet(a), transitions(states) {
    for (std::vector<std::vector<int>> &t : transitions)
        t.resize(a.size() + 1);
}

fsm::Fsm::Fsm(int states, const std::vector<char> &a, std::set<int> s, std::set<int> f)
    : alphabet(a), transitions(states), startingStates(s), finalStates(f) {
    for (std::vector<std::vector<int>> &t : transitions)
        t.resize(a.size() + 1);
}

fsm::Fsm::Fsm(const std::vector<char> &a, const std::vector<std::vector<std::vector<int>>> &t)
    : alphabet(a), transitions(t) {
}

fsm::Fsm::Fsm(const std::vector<char> &a, const std::vector<std::vector<std::vector<int>>> &t, std::set<int> s, std::set<int> f)
    : alphabet(a), transitions(t), startingStates(s), finalStates(f) {
}

fsm::Fsm::Fsm(const Fsm &fsm)
    : alphabet(fsm.alphabet), transitions(fsm.transitions), startingStates(fsm.startingStates), finalStates(fsm.finalStates) {
}

fsm::Fsm::Fsm(Fsm &&fsm)
    : alphabet(std::move(fsm.alphabet)), transitions(std::move(fsm.transitions)), startingStates(std::move(fsm.startingStates)), finalStates(std::move(fsm.finalStates)) {
}

void fsm::Fsm::connect(int s1, int s2, char a) {
    transitions[s1][a == '\0' ? alphabet.size() : std::find(alphabet.begin(), alphabet.end(), a) - alphabet.begin()].push_back(s2);
}

void fsm::Fsm::connect(int s1, int s2, int a) {
    transitions[s1][a].push_back(s2);
}

void fsm::Fsm::start(int state) {
    startingStates.insert(state);
}

void fsm::Fsm::finish(int state) {
    finalStates.insert(state);
}

void fsm::Fsm::inspect() const {
    for (int s1 = 0; s1 < (int)transitions.size(); s1++)
        for (int c = 0; c < (int)alphabet.size() + 1; c++)
            for (int s2 : transitions[s1][c]) {
                printState(s1);

                if (c == (int)alphabet.size())
                    std::cout << " --->> ";
                else
                    std::cout << " --" << alphabet[c] << "-> ";

                printState(s2);

                std::cout << "\n";
            }
}

fsm::Fsm fsm::Fsm::rev() const {
    Fsm rfsm(transitions.size(), alphabet, finalStates, startingStates);

    for (int s1 = 0; s1 < (int)transitions.size(); s1++)
        for (int a = 0; a < (int)alphabet.size() + 1; a++)
            for (int s2 : transitions[s1][a])
                rfsm.connect(s2, s1, a);

    return rfsm;
}

fsm::Fsm fsm::Fsm::det() const {
    const std::vector<std::set<int>> &ec = epsilonClosures();

    std::vector<std::set<int>> q;

    std::set<int> q0;

    for (int s : startingStates)
        q0.insert(ec[s].begin(), ec[s].end());

    q.push_back(q0);

    std::vector<std::vector<std::vector<int>>> t;

    while (t.size() < q.size()) {
        std::vector<std::vector<int>> row;

        for (int a = 0; a < (int)alphabet.size(); a++) {
            std::set<int> ts;

            for (int i : q[t.size()])
                for (int s : transitions[i][a])
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

    std::set<int> f;

    for (int i = 0; i < (int)q.size(); i++)
        for (int s : q[i])
            if (finalStates.find(s) != finalStates.end())
                f.insert(i);

    return Fsm(alphabet, t, {0}, f);
}

fsm::Fsm fsm::Fsm::min() const {
    return rev().det().rev().det();
}

void fsm::Fsm::printState(int state) const {
    std::cout << (startingStates.find(state) != startingStates.end() ? "*" : " ") << state << (finalStates.find(state) != finalStates.end() ? "*" : " ");
}

std::vector<std::set<int>> fsm::Fsm::epsilonClosures() const {
    std::vector<std::set<int>> ec(transitions.size());
    std::vector<bool> flags(transitions.size(), false);

    for (int i = 0; i < (int)ec.size(); i++)
        ec[i].insert(i);

    for (int s = 0; s < (int)transitions.size(); s++)
        epsilonClosure(s, ec, flags);

    return ec;
}

void fsm::Fsm::epsilonClosure(int state, std::vector<std::set<int>> &ec, std::vector<bool> &flags) const {
    if (flags[state])
        return;

    flags[state] = true;

    for (int s : transitions[state][alphabet.size()]) {
        ec[state].insert(s);
        epsilonClosure(s, ec, flags);
    }
}
