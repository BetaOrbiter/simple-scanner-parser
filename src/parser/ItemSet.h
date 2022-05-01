#pragma once
#include <unordered_set>
#include "Item.h"

struct ItemSet{
    std::unordered_set<Item> items;

    void ToClosure();
    bool operator==(const ItemSet& i)const{
        return i.items == items;
    }
};
namespace std {
    template <>
    struct hash<ItemSet>
    {
        size_t operator()(const ItemSet & e) const
        {
            size_t ret=0;
            for(const auto& i:e.items)
                ret ^= std::hash<Item>()(i);
            return ret;
        }
    };
};