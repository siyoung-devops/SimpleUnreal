#pragma once

#include "FProperty.h"
#include "FFunction.h"

#include <string>
#include <vector>

struct FGCReferenceInfo
{
	size_t Offset = 0;
	EGCReferenceKind Kind = EGCReferenceKind::None;
};

class UClass
{
public:
	UClass(const std::string& InName, const UClass* InSuperClass)
		: Name(InName)
		, SuperClass(InSuperClass)
	{
		if (SuperClass != nullptr)
		{
			ReferenceSchema = SuperClass->GetReferenceSchema();
		}
	}

	~UClass();

	UClass(const UClass&) = delete;
	UClass& operator=(const UClass&) = delete;

	const std::string& GetName() const { return Name; }
	const UClass* GetSuperClass() const { return SuperClass; }

	const std::vector<FProperty*>& GetProperties() const { return Properties; }
	const std::vector<FGCReferenceInfo>& GetReferenceSchema() const { return ReferenceSchema; }

	void AddProperty(FProperty* InProperty);
	const FProperty* FindPropertyByName(const std::string& InPropertyName) const;

	void AddFunction(FFunction* InFunction);
	const FFunction* FindFunctionByName(const std::string& InFunctionName) const;

private:
	std::string Name;
	const UClass* SuperClass = nullptr;

	std::vector<FProperty*> Properties;
	std::vector<FFunction*> Functions;

	std::vector<FGCReferenceInfo> ReferenceSchema;
};
