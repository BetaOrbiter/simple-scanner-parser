#pragma once
#include<unordered_map>
#include<unordered_set>
#include"Element.h"
#include"Item.h"
using state_t = int;//dfa状态号类型
constexpr state_t err=-1;//dfa错误状态
constexpr state_t acc=-2;//dfa接受状态
constexpr bool reduce = true;
constexpr bool shift = false;
struct DFANode{
    std::unordered_map<Element, state_t> edges;
    std::unordered_set<Item> items;
    
    bool action;//如为规约项，true否则为false
    Project::project_t hand;//仅在action为true时有效，表示规约的产生式
    //设置action与hand字段
    void SetKind();
};

struct DFA{
    std::unordered_map<state_t, DFANode> nodes;
    state_t start;
    explicit DFA(const Item& start_item);
    friend std::ostream& operator<<(std::ostream& os, const DFA& dfa);
};