#pragma once

#include "FProperty.h"

#include <vector>

class FArrayProperty : public FProperty
{
public:
	FArrayProperty(const std::string& InName, const size_t InOffset, FProperty* InInner)
		: FProperty(InName, InOffset)
		, Inner(InInner)
	{
	}

	~FArrayProperty() override;

	std::string ExportText(const void* InContainer) const override;
	void ImportText(void* InContainer, const std::string& InValue) const override;
	std::string GetTypeName() const override;
	EGCReferenceKind GetReferenceKind() const override;

	virtual size_t GetNum(const void* InContainer) const = 0;
	virtual const void* GetElementPtr(const void* InContainer, const size_t InIndex) const = 0;

private:
	FProperty* Inner = nullptr;
};

template<typename ElementType>
class TArrayProperty : public FArrayProperty
{
public:
	TArrayProperty(const std::string& InName, const size_t InOffset, FProperty* InInner)
		: FArrayProperty(InName, InOffset, InInner)
	{
	}

	size_t GetNum(const void* InContainer) const override
	{
		const std::vector<ElementType>* ArrayPtr = static_cast<const std::vector<ElementType>*>(GetValuePtr(InContainer));

		return ArrayPtr->size();
	}

	const void* GetElementPtr(const void* InContainer, const size_t InIndex) const override
	{
		const std::vector<ElementType>* ArrayPtr = static_cast<const std::vector<ElementType>*>(GetValuePtr(InContainer));

		return &(*ArrayPtr)[InIndex];
	}
};
