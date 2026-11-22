#include "FBoolProperty.h"

#include <stdexcept>

std::string FBoolProperty::ExportText(const void* InContainer) const
{
	const bool* BoolPtr = static_cast<const bool*>(GetValuePtr(InContainer));

	if (*BoolPtr)
	{
		return "true";
	}

	return "false";
}

void FBoolProperty::ImportText(void* InContainer, const std::string& InValue) const
{
	bool* BoolPtr = static_cast<bool*>(GetValuePtr(InContainer));

	if (InValue == "true")
	{
		*BoolPtr = true;
	}
	else if (InValue == "false")
	{
		*BoolPtr = false;
	}
	else
	{
		throw std::invalid_argument("true/false 아님");
	}
}

std::string FBoolProperty::GetTypeName() const
{
	return "bool";
}
