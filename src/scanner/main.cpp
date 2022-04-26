#include "Grammar.h"
#include "FA/NFA.h"
#include "FA/DFA.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

using std::ifstream;
using std::ofstream;
using std::string;
using std::string_view;
using std::map;
using std::vector;
using std::pair;
using std::endl;
using std::getline;

static vector<pair<string,DFA>> dfas;

static void BuildDFA(const string_view grammarFile){
    ifstream txt(grammarFile.data());
    string line;
    map<string,NFA> mm; 
    int i = 0;
    while(getline(txt, line)){
        if(Grammar::IsValid(line)){
            Grammar g(line);
            g.Formalize();
            g.ToSuffix();
            
            NFA m(NFA::CreateNFAFromRegex(g.pattern));
            if(!mm.contains(g.tokenType))
                mm.emplace(g.tokenType, m);
            else
                mm[g.tokenType].Combine(m);
        }
        //if (i++ == 0)
        //    break;
    }
    txt.close();

    for (const auto &i : mm)
        dfas.emplace_back(i.first, DFA(i.second));
    for (int i = 0; i < dfas.size(); ++i)
        if (dfas[i].first == "identifier") {
            dfas[i].swap(dfas.back());
            break;
        }
}

int main(void){
    BuildDFA("lex.txt");
    //std::cout << dfas.at("mmmmm") << std::endl;
    ifstream source("source.c--");
    ofstream out("token.txt");
    string txtLine;
    for(unsigned line=1; getline(source, txtLine); ++line){
        do{
            //删除前导0
            txtLine.erase(0, txtLine.find_first_not_of(' '));

            unsigned token_len = 0;
            auto iter = dfas.cbegin();
            for(;iter!=dfas.cend(); ++iter)
                if((token_len=iter->second.Match(txtLine)) != 0)
                    break;
            
            if(token_len == 0){
                out << "error in line " << line << std::endl;
                exit(1);
            }else{
                out << line << ' ' << iter->first << " " << txtLine.substr(0, token_len) << endl;
                txtLine.erase(0, token_len);
            }
        }while(!txtLine.empty());
    }
    return 0;
}