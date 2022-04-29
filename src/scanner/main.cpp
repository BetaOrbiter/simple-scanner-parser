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
    }
    txt.close();

    for (const auto &i : mm)
        dfas.emplace_back(i.first, i.second);
    for (int i = 0; i < dfas.size(); ++i)
        if (dfas[i].first == "identifier") {
            dfas[i].swap(dfas.back());
            break;
        }
}

int main(void){
    BuildDFA("lex.txt");
    ifstream source("source.c--");
    ofstream out("token.txt");
    string txt_line;
    for(unsigned line=1; getline(source, txt_line); ++line){
        do{
            //删除前导0
            txt_line.erase(0, txt_line.find_first_not_of(' '));

            int best_match_index = -1;
            unsigned token_len = 0;
            for(int i=0;i+1 < dfas.size() ;++i){
                const unsigned now_match_len = dfas.at(i).second.Match(txt_line);
                if(now_match_len > token_len){
                    token_len = now_match_len;
                    best_match_index = i;
                }
            }
            if(best_match_index == -1)
                if((token_len = dfas.back().second.Match(txt_line)) > 0)
                    best_match_index = dfas.size() - 1;
            if(best_match_index == -1){
                out << "error in line " << line << std::endl;
                exit(1);
            }else{
                out << line << ' ' << dfas.at(best_match_index).first << " " << txt_line.substr(0, token_len) << endl;
                txt_line.erase(0, token_len);
            }
        }while(!txt_line.empty());
    }
    return 0;
}