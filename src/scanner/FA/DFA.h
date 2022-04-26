#pragma once
#include <map>
#include "../common.h"
#include "FA.h"

struct NFA;

struct DFANode{
    std::map<char,state_t> edges;
    bool isEnd;
};

struct DFA{
    std::map<state_t, DFANode> nodes;
    state_t start;
    DFA(const NFA& nfa);
    DFA() = delete;
    unsigned Match(const std::string& txt) const;

    friend std::ostream& operator<<(std::ostream& os, const DFA& dfa);
};
std::ostream& operator<<(std::ostream& os, const DFA& dfa);