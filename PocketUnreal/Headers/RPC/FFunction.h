#pragma once

#include "UObject.h"

#include <string>
#include <vector>

class FFunction
{
public:
	FFunction(const std::string& InName)
		: Name(InName)
	{
	}

	virtual ~FFunction() = default;

	FFunction(const FFunction&) = delete;
	FFunction& operator=(const FFunction&) = delete;

	const std::string& GetName() const { return Name; }

	virtual std::string InvokeFromString(UObject* InObject, const std::string& InArgs) const = 0;

protected:
	std::vector<std::string> SplitArgs(const std::string& InArgs, const size_t InArgCount) const;

private:
	std::string Name;
};
