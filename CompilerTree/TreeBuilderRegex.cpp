#include "TreeBuilderRegex.h"

std::list<TreeBuilderRegex::RegexValidate> TreeBuilderRegex::regexes = {
	TreeBuilderRegex::isFor,
	TreeBuilderRegex::isPrint,
	TreeBuilderRegex::isFunction,
	TreeBuilderRegex::isIf,
	TreeBuilderRegex::isProcedure,
	TreeBuilderRegex::isType,
	TreeBuilderRegex::isWhile,
	TreeBuilderRegex::isOpenFigure,
	TreeBuilderRegex::isCloseFigure,
	TreeBuilderRegex::isAssigment,
	TreeBuilderRegex::isBreak,
	TreeBuilderRegex::isContinue,
	TreeBuilderRegex::isCall,
	TreeBuilderRegex::isElse,
};
tree ** TreeBuilderRegex::currentTree = nullptr;

RegexResult TreeBuilderRegex::isType(std::string input)
{
	std::regex reg("^" + joinRegexTypes() + " *[a-zA-Z]+[a-zA-Z0-9]* *(= *[+-]?[a-zA-Z0-9]+.*)?;$");
	if (std::regex_match(input, reg))
		return RegexResult::Type;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isFunction(std::string input)
{
	std::regex rg("^" + compilerInnerData::reserverdWord["function"]
		+ " [a-zA-Z]+[a-zA-Z0-9]*\\((" + joinRegexTypes()
		+ " [a-zA-Z]+[a-zA-Z0-9]*,*)* *(" + joinRegexTypes()
		+ " [a-zA-Z]+[a-zA-Z0-9]*,* *)*\\) -> " + joinRegexTypes() + "$");
	if (std::regex_match(input, rg))
		return RegexResult::Function;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isProcedure(std::string input)
{
	std::regex rg("^" + compilerInnerData::reserverdWord["procedure"]
		+ " [a-zA-Z]+[a-zA-Z0-9]*\\((" + joinRegexTypes()
		+ " [a-zA-Z]+[a-zA-Z0-9]*,*)* *(" + joinRegexTypes()
		+ " [a-zA-Z]+[a-zA-Z0-9]*,* *)*\\)$");
	if (std::regex_match(input, rg))
		return RegexResult::Procedure;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isIf(std::string input)
{
	std::regex rg("^" + compilerInnerData::reserverdWord["if"] + "(.*)");
	if (std::regex_match(input, rg))
		return RegexResult::If;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isElse(std::string input)
{
	std::regex rg("^else");
	if (std::regex_match(input, rg))
		return RegexResult::Else;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isWhile(std::string input)
{
	std::regex rg("^" + compilerInnerData::reserverdWord["while"] + " *\\(.+\\)$");
	if (std::regex_match(input, rg))
		return RegexResult::While;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isFor(std::string input)
{
	std::regex rg("^" + compilerInnerData::reserverdWord["for"] + " *(.*;.*;.*)$");
	if (std::regex_match(input, rg))
		return RegexResult::For;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isOpenFigure(std::string input)
{
	std::regex rg("^\\{$");
	if (std::regex_match(input, rg))
		return RegexResult::FigureOpen;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isCloseFigure(std::string input)
{
	std::regex rg("^\\}$");
	if (std::regex_match(input, rg))
		return RegexResult::FigureClose;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isAssigment(std::string input)
{
	if (*currentTree == nullptr)
		throw std::exception("not found current tree");
	auto vars = (*currentTree)->getListOfAvailableVars();
	std::string varsInStr = joinRegex(vars);
	std::regex rg("^" + varsInStr + " *= *.*;$");
	if (std::regex_match(input, rg))
		return RegexResult::Assigment;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isCall(std::string input)
{
	if (*currentTree == nullptr)
		throw std::exception("not found current tree");
	std::regex rg("^.*(.*);");
	if (std::regex_match(input, rg))
		return RegexResult::Call;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isPrint(std::string input)
{
	if (*currentTree == nullptr)
		throw std::exception("not found current tree");
	std::regex rg("^printf((.*));");
	if (std::regex_match(input, rg))
		return RegexResult::Print;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isBreak(std::string input)
{
	if (*currentTree == nullptr)
		throw std::exception("not found current tree");
	std::regex rg("^break;");
	if (std::regex_match(input, rg))
		return RegexResult::Break;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::isContinue(std::string input)
{
	if (*currentTree == nullptr)
		throw std::exception("not found current tree");
	std::regex rg("^continue;");
	if (std::regex_match(input, rg))
		return RegexResult::Continue;
	return RegexResult::NotClassic;
}

RegexResult TreeBuilderRegex::regex(std::string input)
{
	for (auto func : regexes) {
		auto res = func(input);
		if (res != RegexResult::NotClassic)
			return res;
	}
	return RegexResult::NotClassic;
}