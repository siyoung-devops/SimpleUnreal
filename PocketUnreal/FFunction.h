#pragma once

#include "UObject.h"

#include <string>

class FFunction
{
public:
	FFunction(const std::string& InName)
		: Name(InName)
	{
	}

	virtual ~FFunction() = default;

	const std::string& GetName() const { return Name; }

	virtual std::string InvokeFromString(UObject* InObject, const std::string& InArgs) const = 0;

private:
	std::string Name;
};
