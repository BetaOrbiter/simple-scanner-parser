#pragma once
#include <string>
#include <unordered_set>
#include "Token.h"
//句法的基本单元
struct Element{
    enum Kind:uint8_t{terminal, noTerminal, constant, empty, end};

    static std::unordered_set<Element> terminalSet;
    static std::unordered_set<Element> noTerminalSet;
    static const Element emptyElement;    
    static const Element endElement;
    
    Kind kind;
    std::string value;

    Element(const std::string& v);
    Element(const Kind k, const std::string& v);
    bool operator==(const Element& e) const{
        return (this->kind==e.kind)&&(this->value==e.value);
    }
    bool operator==(const Token& t) const;
    
    friend std::ostream& operator<<(std::ostream& os, const Element& e);
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