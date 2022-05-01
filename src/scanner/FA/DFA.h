#pragma once
#include <map>
#include "../common.h"
#include "FA.h"

struct NFA;

//一个DFA状态
struct DFANode{
    std::map<char,state_t> edges;//出边
    bool isEnd;//是否为终态
};

struct DFA{
    std::map<state_t, DFANode> nodes;//dfa的所有结点
    state_t start;//初始状态号
    //由nfa构造
    DFA(const NFA& nfa);
    DFA() = delete;
    //返回最长匹配长度
    unsigned Match(const std::string& txt) const;

    friend std::ostream& operator<<(std::ostream& os, const DFA& dfa);
};
std::ostream& operator<<(std::ostream& os, const DFA& dfa);