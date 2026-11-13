#pragma once

#include <utility>
#include <string>
#include <vector>

#include "FProperty.h"
#include "FFunction.h"

class UClass
{
public:
	UClass(const std::string& InName, const UClass* InSuperClass, size_t InObjectSize)
		: Name(InName)
		, SuperClass(InSuperClass)
		, ObjectSize(InObjectSize)
	{
	}

	UClass(const UClass&) = delete;
	UClass& operator=(const UClass&) = delete;

	const std::string& GetName() const { return Name; }
	const UClass* GetSuperClass() const { return SuperClass; }
	size_t GetObjectSize() const { return ObjectSize; }

	const std::vector<FProperty*>* GetProperties() const { return &Properties; }
	const std::vector<FProperty*>* GetReferenceProperties() const { return &ReferenceProperties; }

	// Property 관리
	void AddProperty(FProperty* InProperty);
	const FProperty* FindPropertyByName(const std::string& InPropertyName) const;

	// RPC - Function 관리
	void AddFunction(FFunction* InFunction);
	const FFunction* FindFunctionByName(const std::string& InFunctionName) const;

private:
	std::string Name;
	const UClass* SuperClass = nullptr;
	size_t ObjectSize = 0;

	std::vector<FProperty*> Properties;
	std::vector<FProperty*> ReferenceProperties;
	std::vector<FFunction*> Functions;
};