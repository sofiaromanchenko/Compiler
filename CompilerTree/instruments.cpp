#include "instruments.h"
#include "mathParser.h"

std::string createUniqueGUID()
{
	UUID uuid = { 0 };
	std::string guid;
	UuidCreate(&uuid);
	RPC_CSTR szUuid = NULL;
	if (UuidToStringA(&uuid, &szUuid) == RPC_S_OK)
	{
		guid = (char*)szUuid;
		RpcStringFreeA(&szUuid);
	}
	guid.erase(std::remove(guid.begin(), guid.end(), '-'), guid.end());
	if (std::isdigit(guid[0]))
		guid = 'a' + guid;
	return guid;
}
std::string ltrim(std::string str)
{
	auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace(ch); });
	str.erase(str.begin(), it2);
	return str;
}

std::string rtrim(std::string str)
{
	auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace(ch); });
	str.erase(it1.base(), str.end());
	return str;
}

std::string trim(std::string str)
{
	return ltrim(rtrim(str));
}

void createFile(std::string fileName) {
	std::ofstream f(fileName);
	if (!f.is_open())
		exit(EXIT_FAILURE);
	f.close();
}

template<typename T>
std::string join(T b, T e) {
	std::string result;
	for (auto i = b; i != e; i++)
		result += *i;
	return result;
}

template<typename T>
std::string joinRegex(std::list<T> list) {
	if (list.size() == 0)
		return "";
	std::string res = "(";
	for (auto elem : list) {
		res += std::to_string(elem) + "|";
	}
	return res.substr(0, res.length() - 1) + ")";
}

template<>
std::string joinRegex<std::string>(std::list<std::string> list) {
	if (list.size() == 0)
		return "";
	std::string res = "(";
	for (auto elem : list) {
		res += elem + "|";
	}
	return res.substr(0, res.length() - 1) + ")";
}

std::string joinRegexTypes() {
	return joinRegex(compilerInnerData::dataTypes);
}

void copyFiles(std::string from, std::string to)
{
	std::ifstream input(from);
	std::ofstream output(to);
	if (!input.is_open())
		return;

	std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	output << text;

	input.close();
	output.close();
}

std::vector<std::string> splitter(std::string input, char delimiter, filterCustom(std::string))
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(input);
	while (std::getline(tokenStream, token, delimiter))
		tokens.push_back(token);
	if (filter != nullptr)
		std::for_each(tokens.begin(), tokens.end(), [&](std::string & str) { filter(str); });
	return tokens;
}

bool isNumber(std::string input)
{
	std::regex arg("^[0-9]+.?[0-9]*");
	return std::regex_match(input, arg);
}
