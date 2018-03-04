#include "fsm/fsm.hpp"
#include <algorithm>
#include <cstddef>

namespace fsm {

Fsm::Fsm(std::size_t states, const std::vector<symbol_t> &a)
    : m_alphabet(a)
    , m_transitions(states)
{
    for (std::vector<std::vector<state_t>> &t : m_transitions)
    {
        t.resize(a.size() + 1);
    }
}

Fsm::Fsm(
    std::size_t states,
    const std::vector<symbol_t> &a,
    const std::set<state_t> &s,
    const std::set<state_t> &f)
    : m_alphabet(a)
    , m_transitions(states)
    , m_starting_states(s)
    , m_final_states(f)
{
    for (std::vector<std::vector<state_t>> &t : m_transitions)
    {
        t.resize(a.size() + 1);
    }
}

Fsm::Fsm(
    const std::vector<symbol_t> &a,
    const std::vector<std::vector<std::vector<state_t>>> &t)
    : m_alphabet(a)
    , m_transitions(t)
{
}

Fsm::Fsm(
    const std::vector<symbol_t> &a,
    const std::vector<std::vector<std::vector<state_t>>> &t,
    const std::set<state_t> &s,
    const std::set<state_t> &f)
    : m_alphabet(a)
    , m_transitions(t)
    , m_starting_states(s)
    , m_final_states(f)
{
}

void Fsm::connect(state_t s1, state_t s2, symbol_t a)
{
    m_transitions[s1]
                 [a == '\0'
                      ? m_alphabet.size()
                      : std::find(m_alphabet.begin(), m_alphabet.end(), a) -
                            m_alphabet.begin()]
                     .push_back(s2);
}

void Fsm::connect(state_t s1, state_t s2, std::size_t a)
{
    m_transitions[s1][a].push_back(s2);
}

void Fsm::start(state_t state)
{
    m_starting_states.insert(state);
}

void Fsm::finish(state_t state)
{
    m_final_states.insert(state);
}

Fsm Fsm::rev() const
{
    Fsm rfsm(
        m_transitions.size(), m_alphabet, m_final_states, m_starting_states);

    for (state_t s1 = 0; s1 < m_transitions.size(); s1++)
    {
        for (state_t a = 0; a < m_alphabet.size() + 1; a++)
        {
            for (state_t s2 : m_transitions[s1][a])
            {
                rfsm.connect(s2, s1, a);
            }
        }
    }

    return rfsm;
}

Fsm Fsm::det() const
{
    const std::vector<std::set<state_t>> &closures = epsilonClosures();

    std::vector<std::set<state_t>> q;

    std::set<state_t> q0;

    for (state_t s : m_starting_states)
    {
        q0.insert(closures[s].begin(), closures[s].end());
    }

    q.push_back(q0);

    std::vector<std::vector<std::vector<state_t>>> t;

    while (t.size() < q.size())
    {
        std::vector<std::vector<state_t>> row;

        for (std::size_t a = 0; a < m_alphabet.size(); a++)
        {
            std::set<state_t> ts;

            for (state_t i : q[t.size()])
            {
                for (state_t s : m_transitions[i][a])
                {
                    ts.insert(closures[s].begin(), closures[s].end());
                }
            }

            if (ts.empty())
            {
                row.push_back({});
                continue;
            }

            state_t index;

            std::vector<std::set<state_t>>::iterator i =
                std::find(q.begin(), q.end(), ts);

            if (i == q.end())
            {
                index = q.size();
                q.push_back(ts);
            }
            else
            {
                index = i - q.begin();
            }

            row.push_back({index});
        }

        row.push_back({});

        t.push_back(row);
    }

    std::set<state_t> f;

    for (state_t i = 0; i < q.size(); i++)
    {
        for (state_t s : q[i])
        {
            if (m_final_states.find(s) != m_final_states.end())
            {
                f.insert(i);
            }
        }
    }

    return Fsm(m_alphabet, t, {0}, f);
}

Fsm Fsm::min() const
{
    return rev().det().rev().det();
}

std::ostream &operator<<(std::ostream &stream, const Fsm &fsm)
{
    for (Fsm::state_t s1 = 0; s1 < fsm.m_transitions.size(); s1++)
    {
        for (std::size_t a = 0; a < fsm.m_alphabet.size() + 1; a++)
        {
            for (Fsm::state_t s2 : fsm.m_transitions[s1][a])
            {
                fsm.printState(stream, s1);

                if (a == fsm.m_alphabet.size())
                {
                    stream << " --->> ";
                }
                else
                {
                    stream << " --" << fsm.m_alphabet[a] << "-> ";
                }

                fsm.printState(stream, s2);

                stream << std::endl;
            }
        }
    }

    return stream;
}

void Fsm::printState(std::ostream &stream, state_t state) const
{
    if (m_starting_states.find(state) != m_starting_states.end())
    {
        stream << "*";
    }
    else
    {
        stream << " ";
    }

    stream << state;

    if (m_final_states.find(state) != m_final_states.end())
    {
        stream << "*";
    }
    else
    {
        stream << " ";
    }
}

std::vector<std::set<Fsm::state_t>> Fsm::epsilonClosures() const
{
    std::vector<std::set<state_t>> closures(m_transitions.size());
    std::vector<bool> flags(m_transitions.size(), false);

    for (state_t i = 0; i < closures.size(); i++)
    {
        closures[i].insert(i);
    }

    for (state_t s = 0; s < m_transitions.size(); s++)
    {
        buildEpsilonClosures(s, closures, flags);
    }

    return closures;
}

void Fsm::buildEpsilonClosures(
    state_t state,
    std::vector<std::set<state_t>> &closures,
    std::vector<bool> &flags) const
{
    if (flags[state])
    {
        return;
    }

    flags[state] = true;

    for (state_t s : m_transitions[state][m_alphabet.size()])
    {
        closures[state].insert(s);
        buildEpsilonClosures(s, closures, flags);
    }
}

} // namespace fsm
