#pragma once

#include "FProperty.h"

class FStrProperty : public FProperty
{
public:
	FStrProperty(const std::string& InName, const size_t InOffset)
		: FProperty(InName, InOffset)
	{
	}

	std::string ExportText(const void* InContainer) const override;
	void ImportText(void* InContainer, const std::string& InValue) const override;

	std::string GetTypeName() const override;
};
