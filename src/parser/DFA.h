#pragma once
#include<unordered_map>
#include<unordered_set>
#include"Element.h"
#include"Item.h"
using state_t = unsigned;
struct DFANode{
    std::unordered_map<Element, state_t> edges;
    std::unordered_set<Item> items;
};

struct DFA{
    std::unordered_map<state_t, DFANode> nodes;
    state_t start;
    DFA(const Item& start_item);
};