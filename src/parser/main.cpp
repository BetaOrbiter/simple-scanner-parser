#include "State.h"
#include "Project.h"
#include "DFA.h"
#include <iostream>
#include <fstream>
#include <vector>
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::getline;

std::vector<std::vector<state_t>>actGotoTable;

//由dfa构造action_goto表
void BuildTable(const DFA& dfa){
    
}

int main(void){    
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
    Item start_item(0ull, 0u, std::string("$"));
    
    DFA dfa(start_item);
    cout << dfa << std::endl;
    return 0;
}