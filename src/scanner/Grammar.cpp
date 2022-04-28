#include "Grammar.h"
#include "common.h"
#include <stack>

using std::string;
using std::stack;

const std::regex Grammar::grammar_format("<([a-zA-Z0-9_]+)>:=(.+)");

Grammar::Grammar(const string& valid_line){
    std::smatch match;
    std::regex_match(valid_line, match, grammar_format);
    tokenType = match[1].str();
    pattern = match[2].str();
}

void Grammar::ToSuffix(){
    string buffer;
    buffer.reserve(pattern.size());
    stack<char> oper;

    for(string::size_type i=0; i<pattern.size(); ++i){
        if(pattern[i]=='\\'){
            buffer.push_back(pattern[i]);
            buffer.push_back(pattern[++i]);
            continue;
        }

        if(string::npos == controls.find(pattern[i]))
            buffer.push_back(pattern[i]);
        else{
            if(pattern[i] == '(' || pattern[i]=='[')
                oper.push(pattern[i]);
            else if(pattern[i]==')' || pattern[i]==']'){
                const char left = (pattern[i]==')'?'(':'[');
                for(char ch=oper.top(); ch != left; ch = oper.top()){
                    buffer.push_back(ch);
                    oper.pop();
                }
                oper.pop();
            }else{
                if(!oper.empty())            /*如果栈不为空*/ 
                {
                    
                    char ch = oper.top();
                    while(Priority(ch) >= Priority(pattern[i]))    /*弹出栈中优先级大于等于当前运算符的运算符*/ 
                    {
                        buffer.push_back(ch);
                        oper.pop();
                        
                        if(oper.empty())    /*如果栈为空则结束循环*/ 
                        {
                            break;
                        }                                 
                        else ch = oper.top();
                    } 

                    oper.push(pattern[i]);        /*再将当前运算符入栈*/ 
                }
                
                else                /*如果栈为空，直接将运算符入栈*/
                {
                    oper.push(pattern[i]);
                }
            }
        }
    }
    while(!oper.empty()){
        buffer.push_back(oper.top());
        oper.pop();
    }

    pattern = std::move(buffer);
}

void Grammar::Formalize(){
    AddConcatenation();
    HandleRange();
    HandleXor();
}

void Grammar::HandleRange(){
    for(auto i=pattern.find('-'); i!=string::npos; i=pattern.find('-',i+1)){
        if(pattern[i-1] != '\\'){
            string tmp;
            for(auto ch=pattern[i-1];ch<=pattern[i+1];++ch){
                tmp.push_back(ch);
                tmp.push_back('|');
            }
            tmp.pop_back();
            pattern.replace(i-1, 3ULL, tmp);
        }
    }

}

void Grammar::HandleXor(){
    for(auto i=pattern.find('^'); i!=string::npos; i=pattern.find('^', i+1)){
        if(i==0 || pattern[i-1] != '\\'){
            string tmp;
            char victim;

            if(pattern[i+1]=='\\')
                victim = pattern[i+2];
            else
                victim = pattern[i+1];
            
            for(const char ch:alphabet){
                if(ch != victim){
                    if(controls.find(ch)!=string::npos)
                        tmp.push_back('\\');
                    tmp.push_back(ch);
                    tmp.push_back('|');
                }
            }
            tmp.pop_back();
            pattern.replace(i, (pattern[i+1]=='\\'?3:2), tmp);
        }
    }
}

void Grammar::AddConcatenation(){
    constexpr static std::string_view pre(")*+]?");
    constexpr static std::string_view nxt(")*+-]|?&");
    
    auto length = pattern.size();
    bool notControl = false;
    for(decltype(length) i=0; i<length; ++i){
        //转义符处理
        if(pattern[i] == '\\'){
            notControl = true;
            i++;
        }
        else
            notControl = controls.find(pattern[i])==string::npos;
        if(notControl || pre.find(pattern[i])!=string::npos){
            if(i+1<pattern.size() && nxt.find(pattern[i+1]) == string::npos){
                pattern.insert(i+1, 1, '&');
                length++;i++;
            }
        }
    }
}

int Grammar::Priority(const char ch){
    if (ch == '+')
    {
        return 6;
    }
    if (ch == '?')
    {
        return 5;
    }
    if(ch == '*')
    {
        return 4;
    }
    if(ch == '&')
    {
        return 3;
    }
        
    if(ch == '|')
    {
        return 2;
    }
    if (ch == '(')
    {
        return 1;
    }
    if(ch == '[')
    {
        return 0;
    }
    return -1;
}