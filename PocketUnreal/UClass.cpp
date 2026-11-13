#include "UClass.h"

void UClass::AddProperty(FProperty* InProperty)
{
	Properties.push_back(InProperty);

	if (InProperty->HasObjectReference())
	{
		ReferenceProperties.push_back(InProperty);
	}
}

const FProperty* UClass::FindPropertyByName(const std::string& InPropertyName) const
{
	if (Properties.empty())
	{
		return nullptr;
	}

	for (const FProperty* Property : Properties)
	{
		if (Property->GetName() == InPropertyName)
		{
			return Property;
		}
	}

	return nullptr;
}

// RPC
void UClass::AddFunction(FFunction* InFunction)
{
	Functions.push_back(InFunction);
}

const FFunction* UClass::FindFunctionByName(const std::string& InFunctionName) const
{
	if (Functions.empty())
	{
		return nullptr;
	}

	for (const FFunction* Function : Functions)
	{
		if (Function->GetName() == InFunctionName)
		{
			return Function;
		}
	}

	return nullptr;
}