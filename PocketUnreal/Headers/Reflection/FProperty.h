#pragma once

#include <string>

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

	FProperty(const FProperty&) = delete;
	FProperty& operator=(const FProperty&) = delete;

	const std::string& GetName() const { return Name; }
	size_t GetOffset() const { return Offset; }

	const void* GetValuePtr(const void* InContainer) const { return static_cast<const char*>(InContainer) + Offset; }
	void* GetValuePtr(void* InContainer) const { return static_cast<char*>(InContainer) + Offset; }

	virtual std::string ExportText(const void* InContainer) const = 0;
	virtual void ImportText(void* InContainer, const std::string& InValue) const = 0;
	virtual std::string GetTypeName() const = 0;
	virtual EGCReferenceKind GetReferenceKind() const { return EGCReferenceKind::None; }

private:
	std::string Name;
	size_t Offset = 0;
};
