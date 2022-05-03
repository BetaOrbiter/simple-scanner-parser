#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <ostream>
#include "DFA.h"
#include "ItemSet.h"

using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::queue;

DFA::DFA(const Item& start_item){
    ItemSet start_state;
    start_state.items.insert(start_item);
    start_state.ToClosure();

    state_t nxt_state_num=0;
    queue<ItemSet> que;//待处理状态队列
    unordered_map<ItemSet, state_t> toNum;//已出现的状态
    toNum[start_state] = nxt_state_num;
    this->start = nxt_state_num++;
    que.push(std::move(start_state));
    //this->nodes表示以处理完成的状态

    while(!que.empty()){
        ItemSet now;
        std::swap(now, que.front());
        que.pop();
        if(this->nodes.contains(toNum.at(now)))
            continue;
        
        //可转移的符号到可在该符号转移item的映射
        unordered_map<Element, vector<Item> > trans;
        for(const auto &i:now.items)
            if(!i.IsReachLast())
                trans[i.GetNextElement()].push_back(i);
        
        for(const auto&e:trans){
            //构造该符号转移的新状态
            ItemSet new_state;
            for(const auto&i:e.second){
                new_state.items.emplace(i.projectIdx, i.dotPos+1, i.lookAhead);
            }
            new_state.ToClosure();

            //新状态分配新状态号
            if(!toNum.contains(new_state)){
                toNum.emplace(new_state, nxt_state_num++);
                que.push(new_state);
            }
            nodes[toNum.at(now)].edges[e.first]=toNum.at(new_state);
        }
        const state_t tmp = toNum.at(now);
        nodes[tmp].items =std::move(now.items);
        nodes[tmp].SetKind();
    }
}

void DFANode::SetKind(){
    int reduce_item_count=0,shitf_item_count=0;//规约、移进项数量
    Project::project_t reduce_hand = -1;//用什么句法规约
    for(const auto&i:items)
        if(i.IsReachLast()){
            if(reduce_hand==-1)
                reduce_hand = i.projectIdx;
            if(reduce_hand != i.projectIdx){
                //进入该if证明存在不同心的规约项，直接判为归约-归约冲突
                this->action = rr_conflict;
                return;
            }
            reduce_item_count++;
        }else{
            shitf_item_count++;
        }
    this->hand = reduce_hand;

    //归约-归约冲突已在前文处理
    if(shitf_item_count==0)
        this->action = Action::reduce;
    else if(reduce_item_count==0)
        this->action = Action::shitf;
    else
        this->action = Action::sr_conflict;
}

using std::endl;
std::ostream& operator<<(std::ostream& os, const DFA& dfa){
    os << "start state:" << dfa.start << endl<<endl;
    for(const auto& node:dfa.nodes){
        os << "state:" << node.first << " action:" << actionStrings[node.second.action] << endl;
        for(const auto& i:node.second.items){
            os << i << endl;
        }
        for(const auto& edge:node.second.edges){
            os << "on " << edge.first << " to " << edge.second << endl;
        }
        os << endl;
    }
    return os;
}