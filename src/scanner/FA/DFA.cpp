#include <queue>
#include <ostream>
#include "DFA.h"
#include "NFA.h"

using std::set;
using std::map;
using std::queue;
using std::string;
using std::endl;

DFA::DFA(const NFA& nfa){
    //this->nodes表示已构造完成的DFA状态
    
    //为所有出现的nfa状态集合编号,作为DFA状态号
    map<StateSet, state_t> toDFAState;
    state_t nxtDFAState = 0;
    queue<StateSet> que;//宽搜队列
    que.emplace(NFA::GetEpCloure(nfa.head));

    toDFAState.emplace(que.front(), nxtDFAState);
    this->start = nxtDFAState++;

    while(!que.empty()){
        StateSet now;
        now.swap(que.front());
        que.pop();
        if(this->nodes.contains(toDFAState.at(now)))
            continue;
        if (now.contains(nfa.tail))
            this->nodes[toDFAState.at(now)].isEnd = true;

        for(const char ch:alphabet){
            StateSet traget;
            for(const state_t nfaState:now){
                if(nfa[nfaState].input == ch){
                    traget.insert(nfa[nfaState].chTrans);
                }
            }
            traget = NFA::GetEpCloure(traget);
            if(!traget.empty()){
                if (!toDFAState.contains(traget)) {
                    toDFAState.emplace(traget, nxtDFAState++);
                    que.push(traget);
                }
                this->nodes[toDFAState.at(now)].edges.emplace(ch, toDFAState.at(traget));
            }
        }
    }
}

std::ostream& operator<<(std::ostream& os, const DFA& dfa){
    os << "start state:" << dfa.start << endl;
    for (auto &&node : dfa.nodes)
    {
        os << node.first << ' ' << (node.second.isEnd?"true":"false") << endl;
        for (auto &&edge : node.second.edges)
        {
            os << edge.first << ' ' << edge.second << endl;  
        }
        os << endl;
    }
    return os;
}

unsigned DFA::Match(const string& txt) const{
    state_t nowState(start);

    unsigned ret = 0;//最后达到的接受态的路径长度
    for (unsigned i = 0; i < txt.size(); ++i) {
        const auto& edges = nodes.at(nowState).edges;
        //如当前状态有txt[i]对应出边
        if (edges.contains(txt[i])) {
            nowState = edges.at(txt[i]);
            //更新接受态
            if (nodes.at(nowState).isEnd)
                ret = i + 1;
        }
        else
            break;
    }
    return ret;
}