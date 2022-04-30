#pragma once
#include <unordered_set>
#include "Item.h"

struct State{
    std::unordered_set<Item> items;

    void ToClosure();
    bool operator==(const State& i)const{
        return i.items == items;
    }
};
namespace std {
    template <>
    struct hash<State>
    {
        size_t operator()(const State & e) const
        {
            size_t ret=0;
            for(const auto& i:e.items)
                ret ^= std::hash<Item>()(i);
            return ret;
        }
    };
};