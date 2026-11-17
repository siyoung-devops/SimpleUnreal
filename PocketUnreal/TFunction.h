#pragma once
#include "FFunction.h"
#include "TProperty.h"

#include <type_traits>
#include <tuple>
#include <sstream>
#include <stdexcept>

template<typename X>
X FromString(const std::string& InText)
{
	X Value = {};
	TProperty<X> Converter("", 0);
	Converter.ImportText(&Value, InText);
	return Value;
}

template<typename X>
std::string ToString(const X& InValue)
{
	TProperty<X> Converter("", 0);
	return Converter.ExportText(&InValue);
}

template<typename T>
struct TMemberFunctionTraits;

template<typename C, typename... A, typename R>
struct TMemberFunctionTraits<R(C::*)(A...)>
{
	using ClassType = C;

	static constexpr size_t ArgsCount = sizeof...(A);
	using ArgsTuple = std::tuple<std::remove_cvref_t<A>...>;

	static ArgsTuple Parse(const std::vector<std::string>& InPieces)
	{
		size_t Index = 0;
		return ArgsTuple{ FromString<std::remove_cvref_t<A>>(InPieces[Index++])... };
	}

	using ReturnType = R;
};

template<typename C, typename... A, typename R>
struct TMemberFunctionTraits<R(C::*)(A...)const> :TMemberFunctionTraits<R(C::*)(A...)>
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