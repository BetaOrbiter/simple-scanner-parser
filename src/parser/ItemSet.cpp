#include <queue>
#include <unordered_set>
#include "ItemSet.h"


using std::queue;
using std::unordered_set;

void ItemSet::ToClosure(){
    queue<Item> que;
    for(const auto &i:items)
        if(!i.IsReachLast())//dot到最后直接跳过
            if(i.IsNextNoTerminal())
                que.push(i);
    
    //宽搜构造所有item项,items字段为已完成集合
    while(!que.empty()){
        Item now(std::move(que.front()));
        que.pop();
        //已完成的不处理
        if(!items.contains(now))
            items.insert(now);
        //已到规约态或下一个为非终结符直接跳过(没有新item)
        if(now.IsReachLast()||!now.IsNextNoTerminal())
            continue;
        //下一个非终极符可为空特殊处理（越过）
        if(Project::CanEmpty(now.GetNextElement())){
            Item tmp = now;
            tmp.dotPos++;
            que.push(tmp);
        }

        unordered_set<Element> first_set(now.GetFirst());        

        //依次构造新item
        for(const auto p_idx:Project::getBrother(now.GetNextElement())){
            //空产生式直接跳过,否则将变成nfa
            if(Project::IsEmpty(p_idx))
                continue;
            for(const auto &new_lookahead:first_set){
                Item new_item(p_idx, 0, new_lookahead);
                if(!items.contains(new_item)){
                    items.insert(new_item);
                    que.push(new_item);
                }
            }
        }
    }
}
