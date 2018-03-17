#include "fsm/Regex.hpp"
#include <vector>
#include "fsm/Fsm.hpp"

namespace fsm {

class Node
{
public:
    virtual ~Node()
    {
    }

    virtual Fsm compile() = 0;
};

using NodePtr = std::shared_ptr<Node>;

class LiteralNode : public Node
{
public:
    explicit LiteralNode(const std::string &value)
        : m_value{value}
    {
    }

    Fsm compile() override
    {
        return Fsm(0);
    }

private:
    std::string m_value;
};

class WildcardNode : public Node
{
public:
    Fsm compile() override
    {
        return Fsm(0);
    }
};

class ConcatenationNode : public Node
{
public:
    explicit ConcatenationNode(const std::vector<NodePtr> &nodes)
        : m_nodes{nodes}
    {
    }

    Fsm compile() override
    {
        return Fsm(0);
    }

private:
    std::vector<NodePtr> m_nodes;
};

class GroupNode : public Node
{
public:
    explicit GroupNode(const std::vector<NodePtr> &nodes)
        : m_nodes{nodes}
    {
    }

    Fsm compile() override
    {
        return Fsm(0);
    }

private:
    std::vector<NodePtr> m_nodes;
};

class IterationNode : public Node
{
public:
    explicit IterationNode(NodePtr node, bool allow_zero)
        : m_node{node}
        , m_allow_zero{allow_zero}
    {
    }

    Fsm compile() override
    {
        return Fsm(0);
    }

private:
    NodePtr m_node;
    bool m_allow_zero;
};

class OptionalNode : public Node
{
public:
    explicit OptionalNode(NodePtr node)
        : m_node{node}
    {
    }

    Fsm compile() override
    {
        return Fsm(0);
    }

private:
    NodePtr m_node;
};

class RegexParser final
{
public: // methods
    NodePtr parse(const std::string &pattern)
    {
        NodePtr node{new LiteralNode(pattern)};
        return node;
    }

private: // methods
private: // fields
};

class RegexImpl final
{
public: // methods
    RegexImpl(const std::string &pattern)
        : m_fsm{Regex::buildFsm(pattern).min()}
    {
    }

    bool match(const std::string &str)
    {
        return false;
    }

private: // fields
    Fsm m_fsm;
};

Regex::Regex(const std::string &pattern)
    : m_impl{new RegexImpl{pattern}}
{
}

bool Regex::match(const std::string &str)
{
    return m_impl->match(str);
}

Fsm Regex::buildFsm(const std::string &pattern)
{
    return RegexParser().parse(pattern)->compile();
}

} // namespace fsm
