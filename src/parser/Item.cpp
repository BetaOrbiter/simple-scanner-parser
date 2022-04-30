#include "Item.h"

using std::unordered_set;

bool Item::IsNextNoTerminal()const{
    return Project::pool[projectIdx].right[dotPos].kind == Element::noTerminal;
}
const Element& Item::GetNextElement()const{
    return Project::pool[projectIdx].right[dotPos];
}
unordered_set<Element> Item::GetFirst()const{
    const auto& proj = Project::pool[projectIdx];
    if(proj.right.size() == dotPos+1)
        return unordered_set<Element>{lookAhead};
    
    unordered_set<Element> ret;
    size_t pos = dotPos+1;
    for(;pos<proj.right.size(); ++pos){
        if(proj.right[pos].kind != Element::noTerminal){
            ret.insert(proj.right[pos]);
            break;
        }
        auto ss = Project::first[proj.right[pos]];
        ret.merge(std::move(ss));
        if(!Project::first[proj.right[pos]].contains(Element::emptyElement))
            break;
    }
    return ret;
}
Item::Item(const Project::project_t idx, const unsigned dot_pos, const Element& look_ahead)
    :projectIdx(idx),dotPos(dot_pos),lookAhead(look_ahead)
{

}
Project& Item::GetProject()const{
    return Project::pool[this->projectIdx];
}
bool Item::IsReachLast()const{
    return GetProject().right.size() <= dotPos;
}