#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include "DFA.h"
#include "State.h"

using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::queue;

DFA::DFA(const Item& start_item){
    State start_state;
    start_state.items.insert(start_item);
    start_state.ToClosure();

    state_t nxt_state_num=0;
    queue<State> que;//待处理状态队列
    unordered_map<State, state_t> toNum;//已出现的状态
    toNum[start_state] = nxt_state_num;
    this->start = nxt_state_num++;
    que.push(std::move(start_state));
    //this->nodes表示以处理完成的状态

    while(!que.empty()){
        State now;
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
            State new_state;
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
        nodes[toNum.at(now)].items =std::move(now.items);
    }
}