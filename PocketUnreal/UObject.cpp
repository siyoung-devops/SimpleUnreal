#include "UObject.h"
#include "UClass.h"

#include <iostream>

const UClass* UObject::StaticClass()
{
	static UClass ClassInstance = UClass(
		"UObject", nullptr, sizeof(UObject)
	);

	return &ClassInstance;
}

std::vector<UObject*>& UObject::GetObjectList()
{
	static std::vector<UObject*> ObjectList;

	return ObjectList;
}

void UObject::CallFunction(const std::string& InCommand)
{
	size_t SpaceIndex = InCommand.find(' ');

	std::string FunctionName = InCommand.substr(0, SpaceIndex);
	std::string Args = "";

	// 공백이 없으면 npos
	if (SpaceIndex != std::string::npos)
	{
		Args = InCommand.substr(SpaceIndex + 1);
	}

	const FFunction* Function = GetClass()->FindFunctionByName(FunctionName);
	if (Function == nullptr)
	{
		std::cout << "함수없음" << std::endl;
		return;
	}

	try
	{
		std::string ReturnText = Function->InvokeFromString(this, Args);

		if (ReturnText != "")
		{
			std::cout << "Result: " << ReturnText << std::endl;
		}
	}
	catch (const std::exception& Error)
	{
		std::cout << "인자 오류: " << Error.what() << std::endl;
		return;
	}
}