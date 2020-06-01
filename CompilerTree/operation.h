#pragma once
#include <string>
#include "Enums.h"
#include "mathParser.h"
#include "tree.h"
#include "instruments.h"

class TreeBuilderRegex;
class cycleTree;
class tree;

class operation
{
protected:
	 
	std::string lineOnOperation;
	std::string name;
public:
	 
	int order;
	virtual void calculate() = 0;
	virtual std::string toCode(std::string & code) = 0;
	virtual void getData(std::string & code);
	static operation * getOperation(RegexResult, int, std::string, tree* = nullptr);
	operation(std::string line) :lineOnOperation(line) {}
	operation();
	virtual ~operation();
	std::string getName() { return name; }
	Exp * expTree;
};

 
class assigment : public operation {
protected:
	 
	std::string nameOfVal;
public:
	void calculate();
	std::string toCode(std::string & code) {
		auto codeName = expTree->init(nameOfVal, code);
		code += "fstp " + nameOfVal + "\n";
		return nameOfVal;
	}
	assigment(std::string line) : operation(line) {	}
};


class call : public operation {
public:
	call(std::string line) : operation(line) {
		auto indexBrk = line.find_first_of('(');
		auto tmp = line.substr(0, indexBrk);
		name = tmp.substr(tmp.find_last_of(' ') + 1);
	}
	void calculate();
	std::string toCode(std::string & code) {
		code += "";
		return code;
	}
};


class printCall : public call {
public:
	printCall(std::string line) : call(line) {}

	std::string toCode(std::string & code) {
		code += lineOnOperation.substr(0, lineOnOperation.length() - 1);
		return "";
	}
};


class breakCall : public call {
public:
	breakCall(std::string line, cycleTree * treeCycle) : call(line) {
		this->treeCycle = treeCycle;
	}
	std::string toCode(std::string  &code);
	cycleTree * treeCycle;
};


class continueCall : public call {
public:
	continueCall(std::string line, cycleTree * treeCycle) : call(line) {
		this->treeCycle = treeCycle;
	}
	std::string toCode(std::string  &code);
private:
	cycleTree * treeCycle;
};

