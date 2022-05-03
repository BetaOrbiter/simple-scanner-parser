#pragma once
#include<unordered_map>
#include<unordered_set>
#include"Element.h"
#include"Item.h"
using state_t = int;//dfa状态号类型
constexpr state_t err=-1;//dfa错误状态
constexpr state_t acc=-2;//dfa接受状态

//规约项为0，移进项为1，规约规约冲突为2，移进规约冲突为3
enum Action:uint8_t{reduce=0, shitf=1, rr_conflict=2, sr_conflict=3};
constexpr std::string_view actionStrings[]{"reduce","shift","reduce_reduce_conflict","shift_reduce_conflict"};
struct DFANode{
    std::unordered_map<Element, state_t> edges;
    std::unordered_set<Item> items;
    
    Action action;//表示采取的动作或何种冲突
    Project::project_t hand;//仅在action为reduce时有效，表示规约的产生式
    //设置action与hand字段
    void SetKind();
};

struct DFA{
    std::unordered_map<state_t, DFANode> nodes;
    state_t start;
    explicit DFA(const Item& start_item);
    friend std::ostream& operator<<(std::ostream& os, const DFA& dfa);
};