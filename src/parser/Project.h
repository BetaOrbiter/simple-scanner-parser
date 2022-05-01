#pragma once
#include <vector>
#include <regex>
#include <unordered_map>
#include "Element.h"
//一条句法
struct Project{
    using project_t = size_t; //句法句柄类型
    static std::vector<Project> pool;//句法池

    static bool IsValid(const std::string& line);
    static project_t Emplace(const std::string& line);
    //获取同左部的所有句法句柄
    static const std::vector<project_t>& getBrother(const project_t p);
    //获取该左部的所有句法句柄
    static const std::vector<project_t>& getBrother(const Element& e);
    //该非终结符是否可空，仅在EmplaceFirst()后可用
    static bool CanEmpty(const Element& e);
    //该句法是否为空
    static bool IsEmpty(const project_t p);
    //构造first集合
    static void EmplaceFirst();

    Element left;
    std::vector<Element> right;

    Project(const std::string& left,const std::vector<Element>& right);
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
    static std::unordered_map<Element,std::vector<project_t>> mm;
    static void EmplaceSubFirst(const Element& e, std::vector<Element>& path, std::unordered_set<Element>& vis);
};