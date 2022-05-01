#pragma once
#include <string>
#include <vector>
#include "FA.h"
#include "../common.h"

//严格创建的原始NFA,只有一个字符转移
//即state进input转移至chTrans
struct NFAState{
    state_t state;//入口状态号(实际上就是statePool中的下标,该字段无甚作用,这里欠考虑)
    
    char input;     //识别的字符
    state_t chTrans;//目标转移号

    StateSet epTrans;//空转移目标
    //增加一个空转移
    void AddEpTrans(const NFAState& traget);
};

struct NFA{
private:
    static state_t nxt_state;//下一个nfa状态号,等于statePool.size()
    static std::vector<NFAState> statePool;//nfa状态池,外界经由状态号作为下标访问具体状态

public:
    //从后缀表达式构造一个NFA
    static NFA CreateNFAFromRegex(const std::string& suffixPattern);

    //nfa的初始、结束状态号
    state_t head,tail;
    //nfa的初始节点
    NFAState& Head() const;
    //nfa的末尾节点
    NFAState& Tail() const;
    //由nfa状态号获取结点
    NFAState& operator[](const state_t state) const;

    //合并两个nfa(或关系)
    void Combine(NFA&);

    //获取闭包
    static StateSet GetEpCloure(const state_t state);
    //获取闭包
    static StateSet GetEpCloure(const StateSet& stateSet);
    
    //构造一个匹配input字符的nfa
    explicit NFA(const char input);

    NFA();

    friend std::ostream& operator<<(std::ostream& os, const NFA& nfa);
};
std::ostream& operator<<(std::ostream& os, const NFA& nfa);