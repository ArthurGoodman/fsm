#pragma once

#include <map>
#include <ostream>
#include <set>
#include <vector>

namespace fsm {

class Fsm final
{
public: // types
    using state_t = std::size_t;
    using symbol_t = char;

public: // methods
    Fsm(std::size_t states,
        const std::set<state_t> &s = {},
        const std::set<state_t> &f = {});

    Fsm(const std::vector<std::vector<std::set<symbol_t>>> &t,
        const std::set<state_t> &s = {},
        const std::set<state_t> &f = {});

    Fsm(const std::vector<symbol_t> &a,
        const std::vector<std::vector<std::vector<state_t>>> &t,
        const std::set<state_t> &s = {},
        const std::set<state_t> &f = {});

    void connect(state_t s1, state_t s2, symbol_t a);
    void disconnect(state_t s1, state_t s2, symbol_t a);
    void setStarting(state_t state, bool value = true);
    void setFinal(state_t state, bool value = true);

    Fsm rev() const;
    Fsm det() const;
    Fsm min() const;

    friend std::ostream &operator<<(std::ostream &stream, const Fsm &fsm);

private: // methods
    void buildAlphabet();

    void printState(std::ostream &stream, state_t state) const;
    std::vector<std::set<state_t>> epsilonClosures() const;

    void buildEpsilonClosures(
        state_t state,
        std::vector<std::set<state_t>> &ec,
        std::vector<bool> &flags) const;

private: // fields
    std::vector<symbol_t> m_alphabet;
    std::map<symbol_t, std::size_t> m_alphabet_counts;
    std::vector<std::vector<std::set<symbol_t>>> m_transitions;
    std::set<state_t> m_starting_states;
    std::set<state_t> m_final_states;
};

} // namespace fsm
