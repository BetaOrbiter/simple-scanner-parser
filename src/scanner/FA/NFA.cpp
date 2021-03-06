#include <stack>
#include <queue>
#include <ostream>
#include "NFA.h"

using std::stack;
using std::string;
using std::queue;

state_t NFA::nxt_state = 0;
std::vector<NFAState> NFA::statePool;

NFA::NFA(const char input):NFA(){
    statePool[head].input = input;
    statePool[head].chTrans = tail;
}

NFA::NFA(){
    NFAState state1,state2;
    state1.input = '\0';
    state1.chTrans = noState;
    state2.input = '\0';
    state2.chTrans = noState;
    state1.state = head = nxt_state++;
    state2.state = tail = nxt_state++;

    statePool.push_back(std::move(state1));
    statePool.push_back(std::move(state2));
}

NFA NFA::CreateNFAFromRegex(const string& suffixPattern){
    const string& s(suffixPattern);//just for short name
    stack<NFA> nfa_stack;

    for(string::size_type i=0;i<s.size(); ++i){
        if(controls.find(s[i])==string::npos)//非控制符则创建一个新NFA
            nfa_stack.emplace(s[i]);
        else if(s[i]=='\\')//处理转义
            nfa_stack.emplace(s[++i]);
        else if(s[i]=='*'){//闭包运算则向栈顶nfa首尾加入互相空转移
            NFA n(nfa_stack.top());
            n.Head().AddEpTrans(n.Tail());
            n.Tail().AddEpTrans(n.Head());
        }else if(s[i]=='|'){//或运算则构造一个新nfa空转移至栈顶的两个nfa
            NFA n1(nfa_stack.top());
            nfa_stack.pop();
            NFA n2(nfa_stack.top());
            nfa_stack.pop();
            NFA n;

            n.Head().AddEpTrans(n1.Head());
            n.Head().AddEpTrans(n2.Head());
            n1.Tail().AddEpTrans(n.Tail());
            n2.Tail().AddEpTrans(n.Tail());

            nfa_stack.push(std::move(n));
        }else if(s[i] == '&'){//链接符连接栈顶两nfa
            NFA n2(nfa_stack.top());
            nfa_stack.pop();
            NFA n1(nfa_stack.top());
            nfa_stack.pop();

            n1.Tail().AddEpTrans(n2.Head());
            n1.tail = n2.tail;
            nfa_stack.push(std::move(n1));
        }else if(s[i] == '+'){//向栈顶nfa的尾结点加入到头结点的空转移
            NFA n(nfa_stack.top());
            n.Tail().AddEpTrans(n.Head());
        }else if(s[i] == '?'){//向栈顶nfa的头结点加入到尾结点的空转移
            NFA n(nfa_stack.top());
            n.Head().AddEpTrans(n.Tail());
        }
    }

    return nfa_stack.top();//最后的栈顶nfa就是最终nfa
}

NFAState& NFA::Head() const{
    return statePool[head];
}

NFAState& NFA::Tail() const{
    return statePool[tail];
}

void NFAState::AddEpTrans(const NFAState& traget){
    epTrans.insert(traget.state);
}

NFAState& NFA::operator[](const state_t state) const{
    return statePool[state];
}

StateSet NFA::GetEpCloure(const state_t state){
    StateSet ret;
    //宽搜..又是宽搜...
    queue<state_t> que;
    que.push(state);
    while(!que.empty()){
        const state_t now = que.front();
        que.pop();
        if(ret.contains(now))
            continue;
        
        for(const auto s:statePool[now].epTrans)
            if(!ret.contains(s))
                que.push(s);
        ret.insert(now);
    }
    return ret;
}

StateSet NFA::GetEpCloure(const StateSet& stateSet){
    //更上一个不能说十分相似,只能说一毛一样
    StateSet ret;
    queue<state_t> que;
    for(const auto s:stateSet)
        que.push(s);
    while(!que.empty()){
        const state_t now = que.front();
        que.pop();
        if(ret.contains(now))
            continue;
        
        for(const auto s:statePool[now].epTrans)
            if(!ret.contains(s))
                que.push(s);
        ret.insert(now);
    }
    return ret;
}

void NFA::Combine(NFA& other){
    //同CreateNFAFromRegex中对或运算的处理
    NFA n;
    n.Head().AddEpTrans(this->Head());
    n.Head().AddEpTrans(other.Head());
    this->Tail().AddEpTrans(n.Tail());
    other.Tail().AddEpTrans(n.Tail());

    this->head = n.head;
    this->tail = n.tail;
}

std::ostream& operator<<(std::ostream& os, const NFA& nfa)
{
    os << nfa.head << ' ' << nfa.Head().epTrans << std::endl;
    return os;
}
