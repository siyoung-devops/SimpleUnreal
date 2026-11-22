#include "FStrProperty.h"

std::string FStrProperty::ExportText(const void* InContainer) const
{
	const std::string* StrPtr = static_cast<const std::string*>(GetValuePtr(InContainer));

	return *StrPtr;
}

void FStrProperty::ImportText(void* InContainer, const std::string& InValue) const
{
	std::string* StrPtr = static_cast<std::string*>(GetValuePtr(InContainer));

	*StrPtr = InValue;
}

std::string FStrProperty::GetTypeName() const
{
	return "string";
}
