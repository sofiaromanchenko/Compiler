#pragma once
#include <map>
#include <list>
#include <string>
#include <Windows.h>
#include <iostream>
#include <iomanip> 
#include "operation.h"
#include "instruments.h"
#include "mathParser.h"
#include "TreeBuilderRegex.h"

#ifndef TREE_H

class operation;
class assigment;
 
void outputLine(std::ostream& out, std::list<std::string> columnsValue, int = 15);

struct variable {
 
	variable(std::string ID, std::string type) : ID(ID), type(type) {}
	variable(std::string data);
	~variable() { delete treeValue; }
 
	std::string name;
	std::string ID;
 	std::string type;
	std::string value;
	std::string getValueFromData(std::string data);
	
	Exp * treeValue;

	std::string getData() {
		std::string res;
		treeValue->getData(res);
		return res + name + " real8 ?\n";
	}
	
	std::string getInit() {
		if (treeValue == nullptr)
			return "";
		std::string code;
		treeValue->init(name, code);
		code += "fstp " + name + "\n";
		return code;
	}
};


class tree
{
 	tree * parent{ nullptr };
 	std::vector<tree *> children;
 	std::vector<operation*> operations;
	std::map<std::string, variable *> variables;
 	int level{ -1 };
 	std::string name;
 	RegexResult result;
 
	void incrementLevel();
public:
 
	tree * getByName(std::string name);
	 
	void treeChildrenPrinter(int level = 0) {
		auto name = this->name.length() == 0 ? "root" : this->name;
		std::cout << level << ": " << name << std::endl;
		for (auto child : children) {
			child->treeChildrenPrinter(level + 1);
		}
	}
	 
	void optimize();
	 
	bool isBlockList();
	int order;
	tree * findCycles();
 	virtual void createData(std::string& data);
 	void createInit(std::string& data);
 	std::string createCode();
 	friend std::ostream & operator<<(std::ostream& out, const tree * root);
	std::list<std::string> getListOfAvailableVars();
	void outputVars(std::ostream & out);
	tree * getParent();
	tree(int, int, std::string, RegexResult);
	tree(int, int = -1);
	tree * addChildren(tree * tr);
	virtual void createCodeInner(std::string& code);
	virtual ~tree();
	const std::vector<tree *> & getChildren();
	const std::map<std::string, variable *> & getVariables();
	tree * tryGetRoot();
	const variable * operator[](std::string name);
    void addVariable(variable * var);
 	operation* addOperation(std::string operation, int, RegexResult);
	std::list<operation*> getOperationsCalling();
	tree * getLastTree();
};

 
class cycleTree : public tree {
public:
	cycleTree(int i, int order, std::string line, RegexResult reg) :tree(i, order, line, reg) {}
 	std::string end;
 	std::string start;
};

 
class forTree : public cycleTree {
protected:
	Exp* condition;
	operation * first;
	operation * third;
public:
	forTree(int i, int order, std::string line);
	void createCodeInner(std::string& code);
	void createData(std::string & data);
};

 
class whileTree : public cycleTree {
	 
	Exp * condition;
public:
	whileTree(int i, int order, std::string line);
	void createCodeInner(std::string& code);
	void createData(std::string & data);
};

class elseTree : public tree {
public:
	elseTree(int i, int order, std::string line);
	void createCodeInner(std::string& code);
	void createCodeInner(std::string& code, bool f);
};

class ifTree : public tree {
	elseTree * elseTreeInner{ nullptr };
	Exp * condition;
public:
	ifTree(int i, int order, std::string line);
	void createCodeInner(std::string& code);
	void createData(std::string & data);
	void setElse(elseTree * tr) { elseTreeInner = tr; }
};

#endif // !TREE_H

