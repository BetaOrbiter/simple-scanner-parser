#include <queue>
#include <ostream>
#include "DFA.h"
#include "NFA.h"

using std::set;
using std::queue;
using std::endl;

DFA::DFA(NFA& nfa)
    :start(NFA::GetEpCloure(nfa.head)){
    
    queue<StateSet> que;
    que.push(start);
    while(!que.empty()){
        StateSet now;
        now.swap(que.front());
        que.pop();
        if(nodes.contains(now))
            continue;

        for(const char ch:alphabet){
            StateSet traget;
            for(const state_t nfaState:now){
                if(nfa[nfaState].input == ch){
                    traget.insert(nfa[nfaState].chTrans);
                }
            }
            traget = NFA::GetEpCloure(traget);
            if(!traget.empty()){
                if(!nodes.contains(traget))
                    que.push(traget);
                nodes[now].edges[ch].merge(traget);
            }
        }
        if(now.contains(nfa.tail))
            nodes[now].isEnd = true;
    }
}

std::ostream& operator<<(std::ostream& os, const DFA& dfa){
    std::map<StateSet,int> s;int tmp=0;
    for (auto &&i : dfa.nodes)
        s[i.first]=tmp++;
    
    for (auto &&node : dfa.nodes)
    {
        os << s[node.first] << ' ' << (node.second.isEnd?"true":"false") << endl;
        for (auto &&edge : node.second.edges)
        {
            os << edge.first << ' ' << s[edge.second] << endl;  
        }
        os << endl;
    }
    return os;
}