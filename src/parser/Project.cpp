#include "Project.h"
#include <regex>
#include <unordered_set>
#include <algorithm>
#include <ostream>
using std::string;
using std::vector;
using std::unordered_set;

const std::regex Project::projectFormat("(<[a-zA-Z0-9_]+>):=(%[a-zA-Z0-9_]+%|<[a-zA-Z0-9_]+>|#[\\(\\)\\{\\},A-Za-z0-9_<>=!\\+\\-\\*/%&\\|\\^;]+#|@|$){1,}");
std::vector<Project> Project::pool;
std::unordered_map<Element,std::vector<Project::project_t>> Project::fastBro;
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
    fastBro[left].push_back(pool.size()-1);

    return pool.size()-1;
}

const std::vector<Project::project_t>& Project::getBrother(const project_t p){
    return fastBro.at(pool[p].left);
}
const std::vector<Project::project_t>& Project::getBrother(const Element& e){
    return fastBro.at(e);
}
const std::vector<Project::project_t>& Project::getBrother() const{
    return fastBro.at(this->left);
}


void Project::EmplaceFirst(){
    //直接构建终结符first集
    for(const auto&e:Element::terminalSet){
        first[e].insert(e);
    }

    bool done=false;
    //依靠EmplaceSubFirst深度搜索构造first集合
    //大循环的目的在于处理循环依赖
    //当A->..B
    //  B->..AC时
    //如已知A可空,B继续求first(C)
    //否则直接回溯,先求出本轮first(A)
    //在下次大循环,获取A是否可空后继续求新的first(B)进而求出新的first(A)
    //直至所有非终结符first集大小不变
    //精心设计的句法可利用这一漏洞
    while(!done){
        done = true;
        //小循环每次只求一个终结符的部分first集合EmplaceSubFirst深搜的入口终结符
        for(const auto& e:Element::noTerminalSet){
            const unsigned before_size = first[e].size();
            vector<Element> path;//深搜路径
            unordered_set<Element> vis;//深搜已访问标记
            EmplaceSubFirst(e, path, vis);//开始深搜
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
    bool empty_flg=false;//该非终结符是否可空的标记
    //对应每条句法
    for(const auto p:projs){
        bool now_empty_flg=true;//记录该句法是否可空的标记
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
                if(first[*i].contains(Element::emptyElement))
                    continue;
                else{
                    now_empty_flg = false;
                    break;
                }

            //构造该非终结符的子first集合
            EmplaceSubFirst(r, path, vis);

            auto tmp = first.at(r);//临时变量用于消除空转移
            tmp.erase(Element::emptyElement);
            first[path.back()].merge(std::move(tmp));
            if(!first.at(r).contains(Element::emptyElement)){
                now_empty_flg = false;
                break;
            }
        }
        //任意句法可空则非终结符可空
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

std::ostream& operator<<(std::ostream& os, const Project& proj){
    os << proj.left << ":=";
    for(const auto&e:proj.right)
        os << e << ' ';
    return os;
}