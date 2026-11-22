#include "FFloatProperty.h"

#include <stdexcept>

std::string FFloatProperty::ExportText(const void* InContainer) const
{
	const float* FloatPtr = static_cast<const float*>(GetValuePtr(InContainer));

	return std::to_string(*FloatPtr);
}

void FFloatProperty::ImportText(void* InContainer, const std::string& InValue) const
{
	float* FloatPtr = static_cast<float*>(GetValuePtr(InContainer));

	size_t ReadCount = 0;
	float Value = 0.0f;
	try
	{
		Value = std::stof(InValue, &ReadCount);
	}
	catch (const std::exception&)
	{
		throw std::invalid_argument("숫자가 아님");
	}

	if (ReadCount != InValue.size())
	{
		throw std::invalid_argument("숫자가 아님");
	}

	*FloatPtr = Value;
}

std::string FFloatProperty::GetTypeName() const
{
	return "float";
}
