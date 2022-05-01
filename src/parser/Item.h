#pragma once
#include <unordered_set>
#include "Project.h"
//一条lr1句法的item
struct Item{
    Project::project_t projectIdx;//句法句柄
    unsigned dotPos;                 //位置,活前缀(.)位置
    Element lookAhead;
    
    bool operator==(const Item& i)const{
        return this->projectIdx==i.projectIdx && this->dotPos==i.dotPos && this->lookAhead==this->lookAhead;
    }
    //获取dotPos之后元素的first集
    std::unordered_set<Element> GetFirst() const;
    bool IsReachLast()const;
    bool IsNextNoTerminal()const;
    const Element& GetNextElement()const;
    Project& GetProject()const;
    Item(const Project::project_t idx, const unsigned dot_pos, const Element& look_ahead);
    Item();

    friend std::ostream& operator<<(std::ostream& os, const Item& item);
};

namespace std {
    template <>
    struct hash<Item>
    {
        size_t operator()(const Item & i) const
        {
            return std::hash<Project::project_t>()(i.projectIdx) ^ std::hash<unsigned>()(i.dotPos) ^ std::hash<Element>()(i.lookAhead);
        }
    };
};