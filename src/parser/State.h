#pragma once
#include <unordered_set>
#include "Item.h"

struct State{
    std::unordered_set<Item> data;

    void ToClosure();
};