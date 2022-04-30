#include "Project.h"
#include <regex>
#include <unordered_set>
#include <algorithm>
using std::string;
using std::vector;
using std::unordered_set;

const std::regex Project::projectFormat("(<[a-zA-Z0-9_]+>):=(%[a-zA-Z0-9_]+%|<[a-zA-Z0-9_]+>|#[\\(\\)\\{\\},A-Za-z0-9_<>=!\\+\\-\\*/%&\\|\\^;]+#|@|$){1,}");
std::vector<Project> Project::pool;
std::unordered_map<Element,std::vector<Project::project_t>> Project::mm;
std::unordered_map<Element,std::unordered_set<Element>> Project::first;

bool Project::IsValid(const string& line){
    return std::regex_match(line, projectFormat);
}

Project::project_t Project::Emplace(const string& line){
    const std::regex pre("(<[a-zA-Z0-9_]+>).*");
    const std::regex sub(R"((%[a-zA-Z0-9_]+%)|(<[_a-zA-Z0-9]+>|#[\\(\\)\\{\\},A-Za-z0-9_<>=!\\+\\-\\*/%&\\|\\^;]+#|@|$))");
    
    std::smatch match;
    std::regex_match(line, match, pre);
    
    string left(match[1].str()),rest_line(line);
    rest_line.erase(0, left.size()+2);
    
    vector<Element> vec;
    for(std::sregex_iterator i = std::sregex_iterator(rest_line.begin(), rest_line.end(), sub);
        i != std::sregex_iterator();
        ++i)
        if(i->length() >= 1)
        vec.emplace_back(i->str());

    pool.emplace_back(left, std::move(vec));
    mm[left].push_back(pool.size()-1);

    return pool.size()-1;
}

Project::Project(const std::string& left,const std::vector<Element>& right)
    :left(left),right(right)
{

}

const std::vector<Project::project_t>& Project::getBrother(const project_t p){
    return mm.at(pool[p].left);
}
const std::vector<Project::project_t>& Project::getBrother(const Element& e){
    return mm.at(e);
}
const std::vector<Project::project_t>& Project::getBrother() const{
    return mm.at(this->left);
}


void Project::EmplaceFirst(){
    //各符号的first集合大小
    //直接构建终结符first集
    for(const auto&e:Element::terminalSet){
        first[e].insert(e);
    }

    bool done=false;
    while(!done){
        done = true;
        for(const auto& e:Element::noTerminalSet){
            const unsigned before_size = first[e].size();
            vector<Element> path;
            unordered_set<Element> vis;
            EmplaceSubFirst(e, path, vis);
            if(first.at(e).size()>before_size)
                done = false;
        }
    }
}

void Project::EmplaceSubFirst(const Element& e, vector<Element>& path, unordered_set<Element>& vis){
    if(e.kind!=Element::noTerminal || vis.contains(e))
        return;

    path.push_back(e);
    const auto& projs = Project::getBrother(e);
    bool empty_flg=false;
    //对应每条句法
    for(const auto p:projs){
        bool now_empty_flg=true;
        for(const auto&r:Project::pool[p].right){
            if(r.kind != Element::noTerminal){
                now_empty_flg = false;
                first[e].insert(r);
                break;
            }
            
            //非终结符
            if(vis.contains(r))
                break;
            //环形依赖，已经求出可空时继续下一步,否则直接回溯
            auto i = std::find(path.cbegin(), path.cend(), r);
            if(i != path.cend())
                if(first[i->value].contains(Element::emptyElement))
                    continue;
                else
                    break;

            EmplaceSubFirst(r, path, vis);
            auto r_first = first.at(r);
            r_first.erase(Element::emptyElement);
            first[path.back()].merge(std::move(r_first));
            if(!first.at(r).contains(Element::emptyElement)){
                now_empty_flg = false;
                break;
            }
        }
        if(now_empty_flg==true)
            empty_flg = true;
    }
    if(empty_flg == true)
        first[e].insert(Element::emptyElement);
    vis.insert(e);
    path.pop_back();
}

bool Project::IsEmpty()const{
    return right.empty() || right.at(0) == Element::emptyElement;
}
bool Project::IsEmpty(const project_t p){
    return pool[p].IsEmpty();
}

bool Project::CanEmpty(const Element& e){
    return first.at(e).contains(Element::emptyElement);
}