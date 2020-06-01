#pragma once
#include <algorithm>
#include <vector>
#include <fstream>
#include <cctype>
#include <iterator>
#include <sstream>
#include <rpc.h>
#include <Windows.h>
#include <regex>
#include "compilerInnerData.h"
#pragma comment(lib, "Rpcrt4.lib")

 #define filterCustom(type) void (*filter)(##type &) 

std::string createUniqueGUID();
 
std::string ltrim(std::string str);
std::string rtrim(std::string str);
std::string trim(std::string str);

void createFile(std::string fileName);
 
template<typename T>
std::string join(T b, T e);
 
template<typename T>
std::string joinRegex(std::list<T>);
 
std::string joinRegexTypes();
 
void copyFiles(std::string from, std::string to);
 
std::vector<std::string> splitter(std::string input, char delimiter, filterCustom(std::string) = nullptr);
 
bool isNumber(std::string input);