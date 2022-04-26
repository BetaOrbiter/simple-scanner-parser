#pragma once
#include <set>
using state_t = unsigned;
constexpr state_t noState(0xffffffff);
using StateSet = std::set<state_t>;
std::ostream& operator<<(std::ostream& os, const StateSet& set);