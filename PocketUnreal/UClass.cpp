#include "UClass.h"

UClass::~UClass()
{
	for (FProperty* Property : Properties)
	{
		delete Property;
	}

	for (FFunction* Function : Functions)
	{
		delete Function;
	}
}

void UClass::AddProperty(FProperty* InProperty)
{
	Properties.push_back(InProperty);

	EGCReferenceKind Kind = InProperty->GetReferenceKind();
	if (Kind != EGCReferenceKind::None)
	{
		ReferenceSchema.push_back({ InProperty->GetOffset(), Kind });
	}
}

const FProperty* UClass::FindPropertyByName(const std::string& InPropertyName) const
{
	for (const FProperty* Property : Properties)
	{
		if (Property->GetName() == InPropertyName)
		{
			return Property;
		}
	}

	if (SuperClass != nullptr)
	{
		return SuperClass->FindPropertyByName(InPropertyName);
	}

	return nullptr;
}

void UClass::AddFunction(FFunction* InFunction)
{
	Functions.push_back(InFunction);
}

const FFunction* UClass::FindFunctionByName(const std::string& InFunctionName) const
{
	for (const FFunction* Function : Functions)
	{
		if (Function->GetName() == InFunctionName)
		{
			return Function;
		}
	}

	if (SuperClass != nullptr)
	{
		return SuperClass->FindFunctionByName(InFunctionName);
	}

	return nullptr;
}
