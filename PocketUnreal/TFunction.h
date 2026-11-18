#pragma once

#include "FFunction.h"
#include "TProperty.h"

#include <type_traits>
#include <tuple>
#include <sstream>
#include <stdexcept>

template<typename T>
T FromString(const std::string& InText)
{
	T Value = {};
	TProperty<T> Converter("", 0);
	Converter.ImportText(&Value, InText);
	return Value;
}

template<typename T>
struct TMemberFunctionTraits;

template<typename InClassType, typename... InArgTypes, typename InReturnType>
struct TMemberFunctionTraits<InReturnType(InClassType::*)(InArgTypes...)>
{
	using ClassType = InClassType;
	using ReturnType = InReturnType;

	static constexpr size_t ArgsCount = sizeof...(InArgTypes);
	using ArgsTuple = std::tuple<std::remove_cvref_t<InArgTypes>...>;

	static ArgsTuple Parse(const std::vector<std::string>& InPieces)
	{
		size_t Index = 0;
		return ArgsTuple{ FromString<std::remove_cvref_t<InArgTypes>>(InPieces[Index++])... };
	}
};

template<typename InClassType, typename... InArgTypes, typename InReturnType>
struct TMemberFunctionTraits<InReturnType(InClassType::*)(InArgTypes...) const>
	: TMemberFunctionTraits<InReturnType(InClassType::*)(InArgTypes...)>
{
};

template<typename T>
class TFunction : public FFunction
{
public:
	TFunction(const std::string& InName, T InFunction)
		: FFunction(InName)
		, FunctionPointer(InFunction)
	{
	}

	std::string InvokeFromString(UObject* InObject, const std::string& InArgs) const override
	{
		using Traits = TMemberFunctionTraits<T>;

		using ObjectType = typename Traits::ClassType;
		using FuncArgTuple = typename Traits::ArgsTuple;
		using ReturnType = typename Traits::ReturnType;

		ObjectType* Object = dynamic_cast<ObjectType*>(InObject);
		if (Object == nullptr)
		{
			return "";
		}

		std::vector<std::string> ArgTexts;
		std::istringstream Stream(InArgs);
		std::string ArgText;
		while (Stream >> ArgText)
		{
			ArgTexts.push_back(ArgText);
		}

		if (ArgTexts.size() != Traits::ArgsCount)
		{
			throw std::invalid_argument("인자 개수 틀림");
		}

		FuncArgTuple ArgValues = Traits::Parse(ArgTexts);

		if constexpr (std::is_same_v<ReturnType, void>)
		{
			std::apply(
				[&](auto&... Args) { (Object->*FunctionPointer)(Args...); }, ArgValues
			);

			return "";
		}
		else
		{
			using ReturnValueType = std::remove_cvref_t<ReturnType>;
			ReturnValueType ReturnValue = std::apply(
				[&](auto&... Args) { return (Object->*FunctionPointer)(Args...); }, ArgValues
			);

			TProperty<ReturnValueType> ReturnProperty("", 0);
			return ReturnProperty.ExportText(&ReturnValue);
		}
	}

private:
	T FunctionPointer = nullptr;
};
