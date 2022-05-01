#include <ostream>
#include "Item.h"

using std::unordered_set;

bool Item::IsNextNoTerminal()const{
    return this->GetProject().right[dotPos].kind == Element::noTerminal;
}
const Element& Item::GetNextElement()const{
    return this->GetProject().right[dotPos];
}
unordered_set<Element> Item::GetFirst()const{
    const auto& proj = this->GetProject();
    //如活前缀已达最后,特殊处理
    if(proj.right.size() <= dotPos+1)
        return unordered_set<Element>{lookAhead};
    
    unordered_set<Element> ret;
    size_t pos = dotPos+1;
    //依次处理每一个符号
    for(;pos<proj.right.size(); ++pos){
        //非终结符直接返回
        if(proj.right[pos].kind != Element::noTerminal){
            ret.insert(proj.right[pos]);
            break;
        }
        //终结符合并
        auto ss = Project::first[proj.right[pos]];
        ss.erase(Element::emptyElement);//注意去除空符号
        ret.merge(std::move(ss));
        //该终结符无空则停止
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

using std::endl;
std::ostream& operator<<(std::ostream& os, const Item& item){
    os << item.GetProject() << " " << item.dotPos << " " << item.lookAhead;
    return os;
}