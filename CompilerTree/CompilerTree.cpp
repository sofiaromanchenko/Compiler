#include <filesystem>
#include <iostream>
#include "TreeBuilderRegex.h"

tree* treeBuilder(std::ifstream stream);


int main(int argc, char *arg[])
{	if (argc < 1)
		return EXIT_FAILURE;

	std::string fileName = arg[1];
	std::string inputFile = fileName + ".tmp";
	createFile(inputFile);
	copyFiles(fileName, inputFile);

	tree * tr = treeBuilder(std::ifstream(inputFile));
	tr->treeChildrenPrinter();
	std::cout << std::endl;
	tr->optimize();
	tr->treeChildrenPrinter();
	std::cout << std::endl;
	auto code = tr->createCode();
	std::ofstream output(fileName + ".asm");
	output << code;
	output.close();

	system(("nasmCompile.bat " + fileName + ".asm").c_str());

	std::filesystem::remove(inputFile);
	system("pause");
	return 0;
}


void commentEraser(std::string & line, bool & currentState) {
	auto indexOpen = line.find(compilerInnerData::reserverdWord["commentOpen"]);
	auto indexClose = line.find(compilerInnerData::reserverdWord["commentClose"]);

	if (currentState && indexClose == std::string::npos)
		return;

	if (currentState) {
		currentState = false;
		line.erase(0, indexClose + compilerInnerData::reserverdWord["commentClose"].length());
		return;
	}

	while (indexOpen != std::string::npos) {
		currentState = true;
		auto index = line.length();
		if (indexClose != -1) {
			index = indexClose + compilerInnerData::reserverdWord["commentClose"].length();
			currentState = false;
		}
		line.erase(indexOpen, index);

		indexOpen = line.find(compilerInnerData::reserverdWord["commentOpen"]);
		indexClose = line.find(compilerInnerData::reserverdWord["commentClose"]);
	}

}

 
tree* treeBuilder(std::ifstream stream) {
	int level = 0;
	tree * root = new tree(level);
	TreeBuilderRegex::currentTree = &root;
	bool commentState = false;
	std::string tmp;
	operation * operContext = nullptr;
	int order = 0;
	while (std::getline(stream, tmp)) {
		commentEraser(tmp, commentState);
		tmp = trim(tmp);
		if (tmp.length() == 0 || commentState)
			continue;
		auto reg = TreeBuilderRegex::regex(tmp);
		switch (reg) {
		case RegexResult::FigureOpen:
			root = root->addChildren(new tree(order));
			break;
		case RegexResult::FigureClose:
			root = root->getParent();
			break;
		case RegexResult::Type:
			root->addVariable(new variable(tmp));
			break;
		case RegexResult::For:
			root = root->addChildren(new forTree(-1, order, tmp));
			std::getline(stream, tmp);
			break;
		case RegexResult::While:
			root = root->addChildren(new whileTree(-1, order, tmp));
			std::getline(stream, tmp);
			break;
		case RegexResult::Print:
		case RegexResult::Break:
		case RegexResult::Continue:
		case RegexResult::Assigment:
			root->addOperation(tmp, order, reg);
			break;
		case RegexResult::If:
			root = root->addChildren(new ifTree(-1, order, tmp));
			std::getline(stream, tmp);
			break;
		case RegexResult::Else:
			elseTree * elseTreeInner = new elseTree(-1, order, tmp);
			std::getline(stream, tmp);
			if (typeid(*(root->getLastTree())) == typeid(ifTree)) {
				((ifTree*)root->getLastTree())->setElse(elseTreeInner);
				root = root->addChildren(elseTreeInner);
			}
			break;
		}
		order++;
	}
	return root->tryGetRoot();
}
