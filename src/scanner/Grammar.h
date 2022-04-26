#pragma once
#include <string>
#include <regex>
#include <utility>

struct Grammar {//代表文法输入中的一行
    const static std::regex grammar_format;

    static bool IsValid(const std::string& line){
        return std::regex_match(line, grammar_format);
    }

    std::string tokenType;
    std::string pattern;

    Grammar(const std::string& valid_line);

    //中缀转后缀表达式
    void ToSuffix();

    //预处理正则,转为内部标准模式
    void Formalize();

    //控制符优先级
    int Priority(const char ch);

private:
    
    //加入&
    void AddConcatenation();

    //预处理^控制符
    void HandleXor();
    
    //预处理-控制符
    void HandleRange();
};