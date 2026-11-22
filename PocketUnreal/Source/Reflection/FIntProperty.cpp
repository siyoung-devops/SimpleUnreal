#include "FIntProperty.h"

#include <stdexcept>

std::string FIntProperty::ExportText(const void* InContainer) const
{
	const int* IntPtr = static_cast<const int*>(GetValuePtr(InContainer));

	return std::to_string(*IntPtr);
}

void FIntProperty::ImportText(void* InContainer, const std::string& InValue) const
{
	int* IntPtr = static_cast<int*>(GetValuePtr(InContainer));

	size_t ReadCount = 0;
	int Value = 0;
	try
	{
		Value = std::stoi(InValue, &ReadCount);
	}
	catch (const std::exception&)
	{
		throw std::invalid_argument("숫자가 아님");
	}

	if (ReadCount != InValue.size())
	{
		throw std::invalid_argument("숫자가 아님");
	}

	*IntPtr = Value;
}

std::string FIntProperty::GetTypeName() const
{
	return "int";
}
