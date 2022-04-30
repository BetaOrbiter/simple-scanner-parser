#include <queue>
#include <unordered_set>
#include "State.h"

using std::queue;
using std::unordered_set;

void State::ToClosure(){
    queue<Item> que;
    for(const auto &i:data)
        if(!i.IsReachLast())//dot到最后直接跳过
            if(i.IsNextNoTerminal())
                que.push(i);
    
    while(!que.empty()){
        Item now(std::move(que.front()));
        que.pop();
        if(!data.contains(now))
            data.insert(now);
        if(now.IsReachLast()||!now.IsNextNoTerminal())
            continue;
        unordered_set<Element> first_set(now.GetFirst());
        
        //下一个非终极符可为空特殊处理（越过）
        if(Project::CanEmpty(now.GetNextElement())){
            Item tmp = now;
            tmp.dotPos++;
            que.push(tmp);
        }
        for(const auto p_idx:Project::getBrother(now.GetNextElement())){
            if(Project::IsEmpty(p_idx))
                continue;
            for(const auto &new_lookahead:first_set){
                Item new_item(p_idx, 0, new_lookahead);
                if(!data.contains(new_item)){
                    data.insert(new_item);
                    que.push(new_item);
                }
            }
        }
    }
}
