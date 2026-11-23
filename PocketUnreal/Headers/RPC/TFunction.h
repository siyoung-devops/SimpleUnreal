#pragma once

#include "FFunction.h"
#include "CreateProperty.h"

#include <memory>
#include <vector>
#include <type_traits>
#include <utility>

template<typename T>
T FromString(const std::string& InText)
{
	T Value = {};
	static std::unique_ptr<FProperty> TypeProperty(CreateProperty<T>("", 0));
	TypeProperty->ImportText(&Value, InText);
	return Value;
}

template<typename T>
std::string ToString(const T& InValue)
{
	static std::unique_ptr<FProperty> TypeProperty(CreateProperty<T>("", 0));

	return TypeProperty->ExportText(&InValue);
}

template<typename FunctionType, typename ClassType, typename ReturnType, typename... ArgTypes>
class TFunction : public FFunction
{
public:
	TFunction(const std::string& InName, FunctionType InFunction)
		: FFunction(InName)
		, FunctionPointer(InFunction)
	{
	}

	std::string InvokeFromString(UObject* InObject, const std::string& InArgs) const override
	{
		ClassType* Object = static_cast<ClassType*>(InObject);

		std::vector<std::string> ArgTexts = SplitArgs(InArgs, sizeof...(ArgTypes));

		return Invoke(Object, ArgTexts, std::index_sequence_for<ArgTypes...>());
	}

	template<size_t... Indices>
	std::string Invoke(ClassType* InObject, const std::vector<std::string>& InArgTexts, std::index_sequence<Indices...>) const
	{
		if constexpr (std::is_same_v<ReturnType, void>)
		{
			(InObject->*FunctionPointer)(FromString<ArgTypes>(InArgTexts[Indices])...);

			return "";
		}
		else
		{
			ReturnType ReturnValue = (InObject->*FunctionPointer)(FromString<ArgTypes>(InArgTexts[Indices])...);

			return ToString(ReturnValue);
		}
	}

private:
	FunctionType FunctionPointer = nullptr;
};

template<typename ClassType, typename ReturnType, typename... ArgTypes>
FFunction* CreateFunction(const std::string& InName, ReturnType(ClassType::*InFunction)(ArgTypes...))
{
	return new TFunction<decltype(InFunction), ClassType, std::remove_cvref_t<ReturnType>, std::remove_cvref_t<ArgTypes>...>(InName, InFunction);
}

template<typename ClassType, typename ReturnType, typename... ArgTypes>
FFunction* CreateFunction(const std::string& InName, ReturnType(ClassType::*InFunction)(ArgTypes...) const)
{
	return new TFunction<decltype(InFunction), ClassType, std::remove_cvref_t<ReturnType>, std::remove_cvref_t<ArgTypes>...>(InName, InFunction);
}
