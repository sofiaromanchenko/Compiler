#include "tree.h"


void tree::incrementLevel()
{
	this->level++;
	for (tree * tr : children)
		tr->incrementLevel();
}
tree * tree::getByName(std::string name)
{
	if (this->name.find(name) != std::string::npos)
		return this;
	for (auto child : children) {
		auto res = child->getByName(name);
		if (res)
			return res;
	}
	return nullptr;
}

void tree::optimize()
{
	for (int i = 0; i < children.size(); i++) {
		if (children[i]->isBlockList()) {
			children.erase(children.begin() + i);
			i--;
			continue;
		}
		children[i]->optimize();
	}
}

bool tree::isBlockList()
{
	bool result = this->operations.size() == 0;
	std::for_each(children.begin(), children.end(), [&result](tree * tr) { result = result && tr->isBlockList(); });
	return result;
}

tree * tree::findCycles()
{
	if (this->result == RegexResult::For || this->result == RegexResult::While)
		return this;
	if (parent)
		return parent->findCycles();
	return nullptr;
}

void tree::createData(std::string& data)
{
	for (auto var : variables) {
		data += var.second->getData();
	}
	for (auto oper : operations) {
		oper->getData(data);
	}
	std::for_each(children.begin(), children.end(), [&](tree* tr) { tr->createData(data); });
}

void tree::createInit(std::string & data)
{
	for (auto var : variables) {
		data += var.second->getInit();
	}
	std::for_each(children.begin(), children.end(), [&](tree* tr) { tr->createInit(data); });
}

std::string tree::createCode()
{
	std::string code = ".386\n.model flat, stdcall\noption casemap : none\ninclude /masm32/include/kernel32.inc\ninclude /masm32/macros/macros.asm\ninclude /masm32/include/msvcrt.inc\ninclude /masm32/include/masm32.inc\nincludelib /masm32/lib/kernel32\nincludelib /masm32/lib/msvcrt\nincludelib /masm32/lib/masm32";
	code += "\n.data\n";
	createData(code);
	code += "\n.code\n";
	code += "\nstart:\n";
	createInit(code);
	createCodeInner(code);
	code += "exit\nend start\n";
	return code;
}

std::list<std::string> tree::getListOfAvailableVars()
{
	std::list<std::string> result;
	for (auto var : this->variables)
		result.push_back(var.second->name);
	if (parent != nullptr) {
		auto parentVars = parent->getListOfAvailableVars();
		result.insert(result.end(), parentVars.begin(), parentVars.end());
	}
	return result;
}

void tree::outputVars(std::ostream & out)
{
	std::cout << std::endl;
	const int width = 20;
	if (level == 0)
		outputLine(out, { "identif" ,"level", "type", "name", "value" }, width);
	outputLine(out, {});
	for (auto var : this->variables) {
		outputLine(out, { var.second->ID ,std::to_string(this->level), var.second->type, var.second->name, var.second->value }, width);
	}
	for (auto tr : this->children) {
		tr->outputVars(out);
	}
}

tree * tree::getParent()
{
	if (parent == nullptr)
		throw std::exception("not found parent");
	return parent;
}

tree::tree(int level, int order, std::string name, RegexResult result) :name(name), level(level), result(result),
order(order)
{
}

tree::tree(int order, int level) : level(level),
order(order)
{
}

tree* tree::addChildren(tree * tr)
{
	tr->parent = this;
	if (tr->level == -1 || tr->level - this->level == 1)
	{
		tr->level = this->level + 1;
		this->children.push_back(tr);
		return tr;
	}

	if (tr->level > this->level) {
		tr->level = this->level + 1;
		(*this->children.begin())->addChildren(tr);
		return tr;
	}

	if (this->parent == nullptr) {
		tree * newRoot = new tree(-1);
		this->parent = newRoot;
		tr->parent = newRoot;
		newRoot->children.push_back(this);
		newRoot->children.push_back(tr);
		newRoot->incrementLevel();
		return newRoot;
	}
	else
		this->parent->addChildren(tr);
	return tr;
}

void tree::createCodeInner(std::string & code)
{
	int operI = 0, treeI = 0;
	while (operI < operations.size() && treeI < children.size()) {
		if (operations[operI]->order < children[treeI]->order) {
			operations[operI]->toCode(code);
			operI++;
		}
		else {
			children[treeI]->createCodeInner(code);
			treeI++;
		}
	}

	while (operI < operations.size()) {
		operations[operI]->toCode(code);
		operI++;
	}

	while (treeI < children.size()) {
		children[treeI]->createCodeInner(code);
		treeI++;
	}
}

tree::~tree()
{
}

const std::vector<tree*>& tree::getChildren()
{
	return this->children;
}

const std::map<std::string, variable*>& tree::getVariables()
{
	return this->variables;
}

tree * tree::tryGetRoot()
{
	if (this->parent == nullptr)
		return this;
	return parent->tryGetRoot();
}

const variable * tree::operator[](std::string name)
{
	auto index = variables.find(name);
	if (index != variables.end())
		return (*index).second;
	for (tree * tr : children)
	{
		auto indexChildren = (*tr)[name];
		if (indexChildren)
			return indexChildren;
	}
	return nullptr;
}

void tree::addVariable(variable * var)
{
	if (this->variables.find(var->name) != this->variables.end())
		throw new std::exception("Repeate variable");
	this->variables[var->name] = var;
}

operation * tree::addOperation(std::string operation, int order, RegexResult result)
{
	auto oper = operation::getOperation(result, order, operation, this);
	oper->calculate();
	this->operations.push_back(oper);
	return oper;
}

std::list<operation*> tree::getOperationsCalling()
{
	std::list<operation*> res;
	for (auto oper : this->operations)
		if (typeid(*oper) == typeid(call))
			res.push_back(oper);

	for (auto child : this->children) {
		auto childOpers = child->getOperationsCalling();
		res.insert(res.end(), childOpers.begin(), childOpers.end());
	}
	return res;
}

tree * tree::getLastTree()
{
	if (children.size() == 0)
		return nullptr;
	return children[children.size() - 1];
}

void outputLine(std::ostream & out, std::list<std::string> columnsValue, int w)
{
	out << "|";
	for (std::string elem : columnsValue) {
		out << std::setw(w) << elem << "|";
	}
	out << std::endl;
}

std::ostream & operator<<(std::ostream & out, const tree * root)
{
	return out;
}

variable::variable(std::string data)
{
	auto strings = splitter(data, ' ', [](std::string& str) {
		if (str.find(";") == str.length() - 1)
			str.erase(str.length() - 1, 1);
	});

	this->type = strings[0];
	this->name = strings[1];
	this->value = "Not default value";
	if (strings.size() > 2) {
		this->value = getValueFromData(data);
		value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
		mathParser mathParser;
		treeValue = mathParser.MathPars(value);
		std::cout << name << " = ";
		treeValue->print();
		std::cout << std::endl;
	}
	this->ID = createUniqueGUID();
}

std::string variable::getValueFromData(std::string data)
{
	auto strings = splitter(data, '=', [](std::string& str) {
		if (str.find(";") == str.length() - 1)
			str.erase(str.length() - 1, 1);
		str = trim(str);
	});
	return strings[1];
}

forTree::forTree(int i, int order, std::string name) :cycleTree(i, order, name, RegexResult::For)
{
	std::string expression;
	auto firstBracket = name.find_first_of("(") + 1;
	expression = name.substr(firstBracket, name.length() - firstBracket - 1);

	auto operations = splitter(expression, ';');
	mathParser parser;

	if (operations[0].length() == 0 && TreeBuilderRegex::regex(operations[0]) != RegexResult::Assigment)
		throw std::exception();

	if (operations[2].length() == 0 && TreeBuilderRegex::regex(operations[2]) != RegexResult::Assigment)
		throw std::exception();

	first = operation::getOperation(RegexResult::Assigment, 0, operations[0]);
	first->calculate();
	third = operation::getOperation(RegexResult::Assigment, 0, operations[2]);
	third->calculate();
	condition = parser.BoolPars(operations[1]);
}

void forTree::createCodeInner(std::string & code)
{
	std::string guid = createUniqueGUID();
	first->toCode(code);
	start = "for" + guid + ":";
	end = "out" + guid + ":";
	code += "\njmp for" + guid + "\n";
	code += "iter" + guid + ":\n";
	third->toCode(code);
	code += "\njmp for" + guid + "\n";
	code += "condition" + guid + ":\nfinit\n";
	auto res = condition->boolCode(code, "", "out" + guid, "after" + guid);
	int index = code.find("condition" + guid + ":\nfinit\n");
	code.insert(index + std::string("condition" + guid + ":\nfinit\n").length(), "\njmp " + res[0] + "\n");
	index = code.find(res[1]);
	code.insert(index + res[1].length(), "\njmp after" + guid + "\n");
	index = code.find(res[2]);
	code.insert(index + res[2].length(), "\njmp out" + guid + "\n");
	code += "\nfor" + guid + ":\n";
	code += "jmp condition" + guid + "\n";
	code += "after" + guid + ":\n";

	tree::createCodeInner(code);

	code += "jmp iter" + guid + "\n";
	code += "out" + guid + ":\n";
}

void forTree::createData(std::string & data)
{
	tree::createData(data);
	if (condition)
		condition->getData(data);
	if (first)
		first->getData(data);
	if (third)
		third->getData(data);
}

whileTree::whileTree(int i, int order, std::string line) :cycleTree(i, order, line, RegexResult::While)
{
	auto blocks = line.find(compilerInnerData::reserverdWord["while"]);
	condition = mathParser().BoolPars(line.substr(blocks + compilerInnerData::reserverdWord["while"].length()));
}

void whileTree::createCodeInner(std::string & code)
{
	auto guid = createUniqueGUID();
	start = "while" + guid;
	end = "endwhile" + guid;

	code += "\nwhile" + guid + ":\n";
	code += "jmp condition" + guid + "\n";
	code += "after" + guid + ":\n";

	tree::createCodeInner(code);

	code += "\njmp while" + guid + "\n";
	code += "condition" + guid + ":\nfinit\n";
	auto condRes = condition->boolCode(code, "", "endwhile" + guid, "after" + guid);
	int index = code.find("condition" + guid + ":\nfinit\n");
	code.insert(index + std::string("condition" + guid + ":finit\n").length(), "\njmp " + condRes[0] + "\n");
	index = code.find(condRes[1]);
	code.insert(index + condRes[1].length(), "\njmp after" + guid + "\n");
	index = code.find(condRes[2]);
	code.insert(index + condRes[2].length(), "\njmp endwhile" + guid + "\n");

	code += "\nendwhile" + guid + ":\n";
}

void whileTree::createData(std::string & data)
{
	tree::createData(data);
	if (condition)
		condition->getData(data);
}

ifTree::ifTree(int i, int order, std::string line)
	: tree(i, order, line, RegexResult::If)
{
	int start = line.find_first_of('(') + 1;
	int end = line.find_last_of(')');
	mathParser parser;
	condition = parser.BoolPars(line.substr(start, end - start));
}

void ifTree::createCodeInner(std::string & code)
{
	auto guid = createUniqueGUID();
	code += "\nif" + guid + ":\n";
	code += "condition" + guid + ":\nfinit\n";
	auto condRes = condition->boolCode(code, "", "", "");
	int index = code.find(condRes[1]);
	code.insert(index + condRes[1].length(), "\njmp after" + guid + "\n");
	index = code.find(condRes[2]);
	if (elseTreeInner)
		code.insert(index + condRes[2].length(), "\njmp ifelse" + guid + "\n");
	else
		code.insert(index + condRes[2].length(), "\njmp endif" + guid + "\n");
	code += "after" + guid + ":\n";

	tree::createCodeInner(code);

	code += "jmp endif" + guid + "\n";

	if (elseTreeInner) {
		code += "\nifelse" + guid + ":\n";
		elseTreeInner->createCodeInner(code, true);
	}

	code += "\nendif" + guid + ":\n";
}

void ifTree::createData(std::string & data)
{
	tree::createData(data);
	if (condition)
		condition->getData(data);
}

elseTree::elseTree(int i, int order, std::string line)
	:tree(i, order, line, RegexResult::Else)
{
}

void elseTree::createCodeInner(std::string & code)
{
}

void elseTree::createCodeInner(std::string & code, bool f)
{
	tree::createCodeInner(code);
}
