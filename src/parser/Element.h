#pragma once
#include <string>
#include <unordered_set>
#include "Token.h"
//句法的基本单元
struct Element{
    enum Kind:uint8_t{terminal, noTerminal, constant, empty, end};//符号类型

    static std::unordered_set<Element> terminalSet;//终结符集合
    static std::unordered_set<Element> noTerminalSet;//非终结符集合
    static const Element emptyElement;    //空元素,方便使用者,@
    static const Element endElement;      //终结元素,方便使用者,$
    
    Kind kind;
    std::string value;

    //由输入的句法正则式构造
    Element(const std::string& v);
    bool operator==(const Element& e) const{
        return (this->kind==e.kind)&&(this->value==e.value);
    }
    bool operator==(const Token& t) const;//与scanner输出的token比较
    
    friend std::ostream& operator<<(std::ostream& os, const Element& e);

private:
    //直接构造,专属于特殊空和终结元素
    Element(const Kind k, const std::string& v);
};
namespace std {
    template <>
    struct hash<Element>
    {
        size_t operator()(const Element & e) const
        {
            return std::hash<uint8_t>()(e.kind) ^ std::hash<std::string>()(e.value);
        }
    };
};