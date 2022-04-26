#pragma once
#include <string>
#include <vector>
#include "FA.h"
#include "../common.h"

//严格创建的原始NFA,只有一个字符转移
struct NFAState{
    state_t state;
    
    char input;
    state_t chTrans;

    StateSet epTrans;

    void AddEpTrans(const NFAState& traget);
};

class NFA{
    static state_t nxt_state;
    static std::vector<NFAState> statePool;

public:
    static NFA CreateNFAFromRegex(const std::string& suffixPattern);

    state_t head,tail;
    NFAState& Head();
    NFAState& Tail();
    NFAState& operator[](const state_t state);

    void Combine(NFA&);

    static StateSet GetEpCloure(const state_t state);
    static StateSet GetEpCloure(const StateSet& stateSet);
    
    explicit NFA(const char input);

    NFA();
};