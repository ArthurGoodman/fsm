#include "fsm/Regex.hpp"
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>
#include "fsm/Fsm.hpp"

namespace fsm {

#define FOREACH_TEMPLATE_PACK(expression)     \
    {                                         \
        int dummy[] = {((expression), 0)...}; \
        (void)dummy;                          \
    }

class NodePrintContext final
{
public:
    NodePrintContext(std::ostream &stream)
        : m_stream{stream}
        , m_indent{0}
    {
    }

    template <class... Args>
    void print(Args &&... args)
    {
        std::string indent_str(m_indent * 4, ' ');
        m_stream << indent_str;
        FOREACH_TEMPLATE_PACK(m_stream << std::forward<Args>(args))
    }

    void indent()
    {
        m_indent++;
    }

    void unindent()
    {
        m_indent--;
    }

private:
    std::ostream &m_stream;
    std::size_t m_indent;
};

class Node
{
public:
    virtual ~Node()
    {
    }

    virtual void print(NodePrintContext &ctx) = 0;
    virtual Fsm compile() = 0;
};

using NodePtr = std::shared_ptr<Node>;

class CharacterNode : public Node
{
public:
    explicit CharacterNode(char c)
        : m_char{c}
    {
    }

    void print(NodePrintContext &ctx) override
    {
        std::string char_str = (m_char == '"' ? "\\" : "");
        char_str += m_char;
        ctx.print("CharacterNode { \"", char_str, "\" }\n");
    }

    Fsm compile() override
    {
        return Fsm(0);
    }

private:
    char m_char;
};

class CharacterSetNode : public Node
{
public:
    explicit CharacterSetNode(const std::vector<std::pair<char, char>> &sets)
        : m_sets{sets}
    {
    }

    void print(NodePrintContext &ctx) override
    {
        ctx.print("CharacterSetNode {\n");
        ctx.indent();
        for (auto set : m_sets)
        {
            std::string set_str;
            set_str += set.first;
            set_str += '-';
            set_str += set.second;
            ctx.print("Set { ", set_str, " }\n");
        }
        ctx.unindent();
        ctx.print("}\n");
    }

    Fsm compile() override
    {
        return Fsm(0);
    }

private:
    std::vector<std::pair<char, char>> m_sets;
};

class WildcardNode : public Node
{
public:
    void print(NodePrintContext &ctx) override
    {
        ctx.print("WildcardNode {}\n");
    }

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

    void print(NodePrintContext &ctx) override
    {
        ctx.print("ConcatenationNode {\n");
        ctx.indent();
        for (auto node : m_nodes)
        {
            node->print(ctx);
        }
        ctx.unindent();
        ctx.print("}\n");
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

    void print(NodePrintContext &ctx) override
    {
        ctx.print("GroupNode {\n");
        ctx.indent();
        for (auto node : m_nodes)
        {
            node->print(ctx);
        }
        ctx.unindent();
        ctx.print("}\n");
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

    void print(NodePrintContext &ctx) override
    {
        ctx.print("IterationNode {\n");
        ctx.indent();
        m_node->print(ctx);
        ctx.unindent();
        ctx.print("}\n");
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

    void print(NodePrintContext &ctx) override
    {
        ctx.print("OptionalNode {\n");
        ctx.indent();
        m_node->print(ctx);
        ctx.unindent();
        ctx.print("}\n");
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
        m_pattern = pattern;
        m_pos = 0;

        getChar();

        NodePtr node = expr();

        if (!check('\0'))
        {
            throw std::runtime_error(
                "unexpected character '" + std::string(1, std::abs(m_char)) +
                "'");
        }

        return node;
    }

private: // methods
    void getChar()
    {
        static const std::string opeators("+*?.(|)");

        if (m_pos >= m_pattern.size())
        {
            m_char = '\0';
        }
        else if (m_pattern[m_pos] == '\\')
        {
            if (++m_pos >= m_pattern.size())
            {
                throw std::runtime_error("invalid escape sequence");
            }

            m_char = m_pattern[m_pos++];
        }
        else if (opeators.find(m_pattern[m_pos]) != std::string::npos)
        {
            m_char = -m_pattern[m_pos++];
        }
        else
        {
            m_char = m_pattern[m_pos++];
        }
    }

    bool accept(char c)
    {
        if (check(c))
        {
            getChar();
            return true;
        }

        return false;
    }

    bool check(char c)
    {
        return m_char == c;
    }

    NodePtr expr()
    {
        std::vector<NodePtr> nodes;

        while (!check('\0') && !check(-'|') && !check(-')'))
        {
            nodes.emplace_back(suffix());
        }

        return nodes.size() == 1 ? nodes[0]
                                 : NodePtr{new ConcatenationNode(nodes)};
    }

    NodePtr suffix()
    {
        NodePtr node = term();

        while (true)
        {
            if (accept(-'+'))
            {
                node.reset(new IterationNode(node, false));
            }
            else if (accept(-'*'))
            {
                node.reset(new IterationNode(node, true));
            }
            else if (accept(-'?'))
            {
                node.reset(new OptionalNode(node));
            }
            else
            {
                break;
            }
        }

        return node;
    }

    NodePtr term()
    {
        NodePtr node = nullptr;

        // if (accept(-'.'))
        // {
        //     node.reset(new WildcardNode);
        // }
        // else
        if (accept(-'('))
        {
            std::vector<NodePtr> nodes;

            if (!accept(-')'))
            {
                do
                {
                    nodes.emplace_back(expr());
                } while (accept(-'|'));

                if (!accept(-')'))
                {
                    throw std::runtime_error("unmatched parentheses");
                }
            }

            if (nodes.size() == 1)
            {
                node = nodes[0];
            }
            else
            {
                node.reset(new GroupNode(nodes));
            }
        }
        else if (m_char < 0 && !check(-'|') && !check(-')'))
        {
            throw std::runtime_error(
                "unexpected character '" + std::string(1, -m_char) + "'");
        }
        else
        {
            node.reset(new CharacterNode(m_char));
            getChar();
        }

        return node;
    }

private: // fields
    std::string m_pattern;
    std::size_t m_pos;
    char m_char;
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
    NodePtr node = RegexParser().parse(pattern);
    NodePrintContext ctx(std::cout);
    node->print(ctx);
    return node->compile();
}

#undef FOREACH_TEMPLATE_PACK

} // namespace fsm
