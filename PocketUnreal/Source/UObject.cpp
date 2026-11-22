#include "UObject.h"
#include "UClass.h"

#include <iostream>

const UClass* UObject::StaticClass()
{
	static UClass ClassInstance = UClass("UObject", nullptr);

	return &ClassInstance;
}

void UObject::CallFunction(const std::string& InCommand)
{
	size_t NameEndIndex = 0;
	while (NameEndIndex < InCommand.size() && InCommand[NameEndIndex] != ' ')
	{
		++NameEndIndex;
	}

	std::string FunctionName = InCommand.substr(0, NameEndIndex);
	std::string Args;

	if (NameEndIndex < InCommand.size())
	{
		Args = InCommand.substr(NameEndIndex + 1);
	}

	const FFunction* Function = GetClass()->FindFunctionByName(FunctionName);
	if (Function == nullptr)
	{
		std::cout << "함수 없음 : " << FunctionName << std::endl;
		return;
	}

	try
	{
		std::string ReturnText = Function->InvokeFromString(this, Args);

		if (!ReturnText.empty())
		{
			std::cout << FunctionName << " Result : " << ReturnText << std::endl;
		}
	}
	catch (const std::exception& Error)
	{
		std::cout << "인자 오류 : " << Error.what() << std::endl;
	}
}
