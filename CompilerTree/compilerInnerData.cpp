#include "compilerInnerData.h"

std::list<std::string> compilerInnerData::dataTypes = { "int", "double" };

std::map<std::string, std::string> compilerInnerData::reserverdWord = {
	{"function", "func"},
	{"procedure", "proc"},
	{"if", "if"},
	{"else", "else"},
	{"while", "while"},
	{"foreach", "for"},
	{"commentOpen", "/*"},
	{"commentClose", "*/"}

};
