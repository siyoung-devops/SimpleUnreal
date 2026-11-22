#include "FFunction.h"

#include <sstream>
#include <stdexcept>

std::vector<std::string> FFunction::SplitArgs(const std::string& InArgs, const size_t InArgCount) const
{
	std::vector<std::string> ArgTexts;
	std::istringstream Stream(InArgs);
	std::string ArgText;
	while (Stream >> ArgText)
	{
		ArgTexts.push_back(ArgText);
	}

	if (ArgTexts.size() != InArgCount)
	{
		throw std::invalid_argument("인자 개수 틀림");
	}

	return ArgTexts;
}
