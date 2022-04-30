#include "Element.h"
using std::string;

const Element Element::emptyElement{Element::empty, string("@")};
const Element Element::endElement{Element::end, string("$")};
std::unordered_set<Element> Element::terminalSet{Element::emptyElement,Element::endElement};
std::unordered_set<Element> Element::noTerminalSet;

Element::Element(const string& v){
    if(v.size()==1){
        if(v.front()==endElement.value.front()){
            this->kind = endElement.kind;
            this->value = endElement.value;
        }
        else{
            this->kind = emptyElement.kind;
            this->value = emptyElement.value;
        }
        return;
    }
    const char op = v.at(0);
    this->value = v.substr(1, v.size()-2);
    if(op == '<'){
        this->kind = noTerminal;
        noTerminalSet.emplace(*this);
    }
    else{ 
        if(op == '%'){
            this->kind = terminal;
        }
        else if(op == '#'){
            this->kind = constant;
        }
        terminalSet.emplace(*this);
    }
}
Element::Element(const Kind k, const string& v)
    :kind(k),value(v)    
{

}

bool Element::operator==(const Token& t){
    if(kind == noTerminal)
        return false;
    else if(kind == terminal)
        return value == t.type;
    else if(kind == constant)
        return value == t.value;
    else if(kind == empty)
        return false;
    else if(kind == end)
        return t.type=="end";
    return false;
};
