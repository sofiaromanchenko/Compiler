#include "operation.h"

void operation::getData(std::string & code)
{
	if (expTree)
		expTree->getData(code);
}

operation * operation::getOperation(RegexResult result, int order, std::string line, tree * tr)
{
	operation * oper = nullptr;
	switch (result) {
	case RegexResult::Assigment:
		oper = new assigment(line);
		break;
	case RegexResult::Print:
		oper = new printCall(line);
		break;
	case RegexResult::Continue:
		oper = new continueCall(line, (cycleTree*)tr->findCycles());
		break;
	case RegexResult::Break:
		oper = new breakCall(line, (cycleTree*)tr->findCycles());
		break;
	}
	if (oper)
		oper->order = order;
	return oper;
}

operation::operation()
{
}


operation::~operation()
{
	delete expTree;
}

void assigment::calculate()
{
	auto parts = splitter(lineOnOperation, '=', [](std::string & str) {
		if (str.find(";") != std::string::npos)
			str.erase(str.end() - 1);
		str = trim(str);
	});
	auto assign = parts[1];
	assign.erase(std::remove(assign.begin(), assign.end(), ' '), assign.end());
	mathParser math;
	expTree = math.MathPars(assign);
	std::cout << parts[0] << " = ";
	this->nameOfVal = parts[0];
	expTree->print();
	std::cout << std::endl;
}

void call::calculate()
{
}

std::string breakCall::toCode(std::string & code)
{
	code += "\njmp " + treeCycle->end + "\n";
	return "";
}

std::string continueCall::toCode(std::string & code)
{
	code += "\njmp " + treeCycle->start + "\n";
	return "";
}

