#include "mathParser.h"

Exp * mathParser::MathPars(std::string math)
{
	int level = 0;
	for (int i = math.size() - 1; i >= 0; --i) {
		char c = math[i];
		if (c == ')') {
			++level;
			continue;
		}
		if (c == '(') {
			--level;
			continue;
		}
		if (level > 0) continue;
		if ((c == '+' || c == '-') && i != 0) {
			std::string left(math.substr(0, i));
			std::string right(math.substr(i + 1));
			return new Node(c, MathPars(left), MathPars(right));
		}
	}
	for (int i = math.size() - 1; i >= 0; --i) {
		char c = math[i];
		if (c == ')') {
			++level;
			continue;
		}
		if (c == '(') {
			--level;
			continue;
		}
		if (level > 0) continue;
		if (c == '*' || c == '/') {
			std::string left(math.substr(0, i));
			std::string right(math.substr(i + 1));
			return new Node(c, MathPars(left), MathPars(right));
		}
	}
	if (math[0] == '(') {
		for (int i = 0; i < math.size(); ++i) {
			if (math[i] == '(') {
				++level;
				continue;
			}
			if (math[i] == ')') {
				--level;
				if (level == 0) {
					std::string exp(math.substr(1, i - 1));
					return MathPars(exp);
				}
				continue;
			}
		}
	}
	else
		
		return new Term(math);
	std::cerr << "Error:never execute point" << std::endl;
	return NULL;
}

Exp * mathParser::BoolPars(std::string math)
{
	int level = 0;
	for (int i = math.size() - 1; i >= 0; --i) {
		char c = math[i];

		if (c == '!') {
			return new OneNode(BoolPars(math.substr(1)), c);
		}

		if (c == ')') {
			++level;
			continue;
		}
		if (c == '(') {
			--level;
			continue;
		}
		if (level > 0) continue;
		if ((c == '<' || c == '>') && i != 0) {
			std::string left(math.substr(0, i));
			std::string right(math.substr(i + 1));
			return new Node(c, BoolPars(left), BoolPars(right));
		}
	}
	for (int i = math.size() - 1; i >= 0; --i) {
		char c = math[i];
		if (c == ')') {
			++level;
			continue;
		}
		if (c == '(') {
			--level;
			continue;
		}
		if (level > 0) continue;
		if (c == '|' || c == '&') {
			std::string left(math.substr(0, i));
			std::string right(math.substr(i + 1));
			return new Node(c, BoolPars(left), BoolPars(right));
		}
	}
	if (math[0] == '(') {
		for (int i = 0; i < math.size(); ++i) {
			if (math[i] == '(') {
				++level;
				continue;
			}
			if (math[i] == ')') {
				--level;
				if (level == 0) {
					std::string exp(math.substr(1, i - 1));
					return BoolPars(exp);
				}
				continue;
			}
		}
	}
	else
		return new Term(math);
	std::cerr << "Error:never execute point" << std::endl;
	return NULL;
}
