#include "Grammar.h"
#include "FA/NFA.h"
#include "FA/DFA.h"
#include <iostream>
#include <fstream>
int main(void){
    std::ifstream txt("lex.txt");
    std::string line;
    std::map<std::string,NFA> mm; 
    while(std::getline(txt, line)){
        if(Grammar::IsValid(line)){
            Grammar g(line);
            g.Formalize();
            g.ToSuffix();
            
            NFA m(NFA::CreateNFAFromRegex(g.pattern));
            if(!mm.contains(g.tokenType))
                mm[g.tokenType] = m;
            else
                mm[g.tokenType].Combine(m);
        }
    }
    for (auto &&scanner : mm)
    {
        std::cout << scanner.first << std::endl;
        std::cout << scanner.second << std::endl;
    }
    
    return 0;
}