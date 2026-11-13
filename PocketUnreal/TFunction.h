#pragma once
#include "FFunction.h"
#include "TProperty.h"

#include <type_traits>
#include <tuple>
#include <sstream>
#include <stdexcept>

/*
std::remove_reference_t<X>	&만 벗김	const std::string
std::remove_const_t<X>	맨 바깥 const만 벗김	그대로
std::remove_cvref_t<X> (C++20)	&와 const 둘 다 벗김	std::string
std::decay_t<X>	둘 다 벗기고, 배열·함수는 포인터로 바꿈	std::string
*/

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

// "SetInfo Lee 41"
template<typename... A>
std::string MakeCommand(const std::string& InFunc, const A&... InArgs)
{
	std::string Command = InFunc;

	// ... = 컴파일러가 대신 하는 복사·붙여넣기
	((Command += " " + ToString(InArgs)), ...);
	return Command;
}

template<typename T>
struct TMemberFunctionTraits;

template<typename C, typename... A, typename R>
struct TMemberFunctionTraits<R(C::*)(A...)>
{
	using ClassType = C;

	// 인자 하나일 때 using ArgType = A;
	// 인자 여러개일 때
	static constexpr size_t ArgsCount = sizeof...(A);
	using ArgsTuple = std::tuple<std::remove_cvref_t<A>...>;

	static ArgsTuple Parse(const std::vector<std::string>& InPieces)
	{
		size_t Index = 0;
		return ArgsTuple{ FromString<std::remove_cvref_t<A>>(InPieces[Index++])... };
	}

	using ReturnType = R;
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

	// 글자 인자를 값으로 바꿔서 오브젝트에 대해 함수를 실행한다
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

		// 인자가 여러개일때, 공백 기준으로 나눠서 저장
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

		// void 형식일 때
		if constexpr (std::is_same_v<ReturnType, void>)
		{
			// std::apply가 tuple을 받아서 안의 값들을 함수 인자로 펼쳐줌
			// std::get<0>(t), std::get<1>(t),,,
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