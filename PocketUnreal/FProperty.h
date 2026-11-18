#pragma once

#include "UObject.h"

#include <string>
#include <vector>

enum class EGCReferenceKind
{
	None,
	Object,
	ObjectArray
};

class FProperty
{
public:
	FProperty(const std::string& InName, const size_t InOffset)
		: Name(InName)
		, Offset(InOffset)
	{
	}

	virtual ~FProperty() = default;

	const std::string& GetName() const { return Name; }
	size_t GetOffset() const { return Offset; }

	virtual std::string ExportText(const void* InContainer) const = 0;
	virtual void ImportText(void* InContainer, const std::string& InValue) const = 0;
	virtual std::string GetTypeName() const = 0;
	virtual EGCReferenceKind GetReferenceKind() const = 0;

private:
	std::string Name;
	size_t Offset = 0;
};
