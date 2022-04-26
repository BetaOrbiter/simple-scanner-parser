#include <ostream>
#include "FA.h"

using std::endl;

std::ostream& operator<<(std::ostream& os, const StateSet& set){
    os << '{';
    for (auto &&i : set)
        os << ' ' << i;
    os << '}';
    return os;
}