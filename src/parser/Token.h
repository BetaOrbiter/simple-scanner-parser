#include <string>
struct Token{
    unsigned line;
    std::string type;
    std::string value;
    //输入合法性由scanner保证
    Token(const std::string& line){
        auto blank1 = line.find(' ', 0);
        auto blank2 = line.find(' ', blank1+1);
        this->line = std::stoi(line.substr(0, blank1));
        this->type = line.substr(blank1+1, blank2-blank1-1);
        this->value = line.substr(blank2+1);
    }
    Token(){}
};