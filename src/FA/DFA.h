#pragma once
#include <map>
#include "../common.h"
#include "FA.h"

struct NFA;

struct DFANode{
    std::map<char,StateSet> edges;
    bool isEnd;
};

struct DFA{
    std::map<StateSet, DFANode> nodes;
    StateSet start;
    DFA(NFA& nfa);

    friend std::ostream& operator<<(std::ostream& os, const DFA& dfa);
};
std::ostream& operator<<(std::ostream& os, const DFA& dfa);