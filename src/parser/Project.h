#pragma once
#include <vector>
#include <regex>
#include <unordered_map>
#include "Element.h"
//句法类
struct Project{
    //句法句柄类型，使用者持有，由pool获取实例
    using project_t = size_t;
    //句法池，使用者由此获取句法
    static std::vector<Project> pool;

    //检查line是否是一个合法产生式
    static bool IsValid(const std::string& line);
    //由line构造一个产生式并加入存储池中
    static project_t Emplace(const std::string& line);
    //获取同左部的所有句法句柄
    static const std::vector<project_t>& getBrother(const project_t p);
    //获取该左部的所有句法句柄
    static const std::vector<project_t>& getBrother(const Element& e);
    //该非终结符是否可空，仅在EmplaceFirst()后可用
    static bool CanEmpty(const Element& e);
    //该句法是否为空
    static bool IsEmpty(const project_t p);
    //标记产生式读入完毕，并构造各符号first集
    static void EmplaceFirst();

    Element left;//左部
    std::vector<Element> right;//右部

    //获取同左部的所有句法句柄
    const std::vector<project_t>& getBrother() const;
    //该句法是否为空
    bool IsEmpty()const;
    //符号到first集合的映射
    static std::unordered_map<Element,std::unordered_set<Element>> first;

    friend std::ostream& operator<<(std::ostream& os, const Project& proj);
private:
    const static std::regex projectFormat;
    //快速获取左部相同句法
    static std::unordered_map<Element,std::vector<project_t>> fastBro;
    //构造first集的辅助函数
    static void EmplaceSubFirst(const Element& e, std::vector<Element>& path, std::unordered_set<Element>& vis);
};