#include "ItemSet.h"
#include "Project.h"
#include "DFA.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <format>
#include <utility>

using std::format;
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::getline;
using std::pair;

std::vector<std::vector<state_t>>gotoTable;
std::vector<pair<bool,Project::project_t>> actionTable;//同dfaNode中的action、hand字段
std::unordered_map<Element, int> toColumn;//符号到goto表列数的映射

//由dfa构造action_goto表,各列顺序与[Element::TerminalSet : Element::noterminalSet]顺序相同
static void BuildTable(const DFA& dfa){
    //配置表大小并预分配error状态(-1)或shift状态
    gotoTable.resize(dfa.nodes.size());
    const size_t column_size = Element::noTerminalSet.size()+Element::terminalSet.size();
    for(auto &row:gotoTable){
        row.assign(column_size, err);
    }
    actionTable.reserve(dfa.nodes.size());

    //构造action表
    for(int i=0;i<dfa.nodes.size();++i)
        actionTable.emplace_back(dfa.nodes.at(i).action, dfa.nodes.at(i).hand);

    //构造goto表
    for(int i=0;i<gotoTable.size();++i){
        //注意各列顺序与Element维护的符号unordered_set顺序相同,先终结符后非终结符
        int column=0;
        //终结符集合
        for(auto j=Element::terminalSet.cbegin();j!=Element::terminalSet.cend();++j){
            const auto& dfa_node = dfa.nodes.at(i);
            if(dfa_node.edges.contains(*j))
                gotoTable[i][column] = dfa_node.edges.at(*j);
            toColumn.emplace(*j, column);
            column++;
        }
        //非终结符集合
        for(auto j=Element::noTerminalSet.cbegin();j!=Element::noTerminalSet.cend();++j){
            const auto& dfa_node = dfa.nodes.at(i);
            if(dfa_node.edges.contains(*j))
                gotoTable[i][column] = dfa_node.edges.at(*j);
            toColumn.emplace(*j, column);
            column++;
        }
    }    
    //接受态特殊处理
    //第一个句法的第一个右部符号所在列(第一个句法一定为起始句法)
    gotoTable[dfa.start][toColumn.at(Project::pool.at(0).right.at(0))] = acc;
}

//打印goto表
static void PrintTable(std::ostream& os){
    size_t column_len=0;//一列的宽度
    for(const auto&e:Element::terminalSet)
        column_len = std::max(column_len, e.value.size());    
    for(const auto&e:Element::noTerminalSet)
        column_len = std::max(column_len, e.value.size());
    column_len = std::max(column_len, 5ULL);
    column_len+=2;

    //构造格式控制字段
    string form = "{:<";
    form.append(std::to_string(column_len));
    form.push_back('}');

    //goto表
    os << "goto table" << endl;
    os << format(form, "state");
    for(const auto&e:Element::terminalSet)
        os << format(form, e.value);
    for(const auto&e:Element::noTerminalSet)
        os << format(form, e.value);
    os << endl;

    for(int i=0;i<gotoTable.size();++i){
        os << format(form, i);
        for(const auto n:gotoTable.at(i)){
            if(n==err)
                os << format(form, "");
            else if(n==acc)
                os << format(form, "acc");
            else
                os << format(form, n);
        }
        os << endl;
    }

    //action表
    os << "action table" << endl;
    os << format(form, "state") << endl;
    for(int i=0;i<actionTable.size();++i){
        os << format(form, i);
        os << format(form, actionTable.at(i).first==shift?"shitf":"reduce");
        if(actionTable.at(i).first==reduce)
            os << Project::pool[actionTable.at(i).second];
        os << endl;
    }
}

//利用action_goto表解析token序列
static bool Parse(std::string_view token_file_name,const state_t start, std::ostream& os){
    ifstream token_file(token_file_name.data());
    std::vector<state_t> state_stack;//状态栈
    std::vector<Element> ele_stack;//符号栈
    state_stack.push_back(start);
    
    while(state_stack.back()!=err&&state_stack.back()!=acc){
        state_t now_state = state_stack.back();

        //打印符号栈状态栈
        os << "state stack:  ";
        for(const auto state:state_stack)
            os << '[' << state << ']';
        os << endl << "element stack:";
        for(const auto& ele:ele_stack)
            os << ele << ' ';
        os << endl;

        if(actionTable.at(now_state).first == reduce){
            const Project& proj = Project::pool.at(actionTable.at(now_state).second);
            //规约相应多的过程
            for(int i=0;i<proj.right.size();++i){
                state_stack.pop_back();
                ele_stack.pop_back();
            }
            ele_stack.push_back(proj.left);
            now_state = state_stack.back();
            state_stack.push_back(gotoTable.at(now_state).at(toColumn.at(proj.left)));
        }else{//shift过程
            string token_line;
            getline(token_file, token_line);
            Token token(token_line);
            Element match("$");
            //寻找可与token匹配的element
            for(const auto&e:Element::terminalSet)
                if(e == token)
                    match = e;
            
            state_stack.push_back(gotoTable.at(now_state).at(toColumn.at(match)));
            ele_stack.push_back(match);
        }
    }
    //结束前打印符号栈状态栈
    os << "state stack:  ";
    for(const auto state:state_stack)
        os << '[' << state << ']';
    os << endl << "element stack:";
    for(const auto& ele:ele_stack)
        os << ele << ' ';
    os << endl;

    return state_stack.back()==acc;
}

int main(void){
    //要求第一个句法为起始符号句法(本程序不会再构造一个起始符号)
    
    //读入、解析句法，构造符号first集
    ifstream grammar_file("grammar.txt");
    string s;
    while(getline(grammar_file, s)){
        if(Project::IsValid(s)){
            auto hand = Project::Emplace(s);
        }else{
            cout << "无效句法:" << s << endl; 
            return 1;
        }
    }
    Project::EmplaceFirst();
    
    //构造dfa与action_goto表
    Item start_item(0ull, 0u, std::string("$"));
    DFA dfa(start_item);
    BuildTable(dfa);

    //输出自动机、表    
    std::ofstream dfa_file("paser_dfa.txt");
    dfa_file << dfa << std::endl;
    std::ofstream goto_file("action_goto_table.txt");
    PrintTable(goto_file);

    //解析
    std::ofstream process_file("process.txt");
    process_file << (Parse("token.txt", dfa.start, process_file)?"acc":"err") << endl;
    return 0;
}