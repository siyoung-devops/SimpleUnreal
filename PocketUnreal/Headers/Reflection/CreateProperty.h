#pragma once

#include "UObject.h"
#include "FIntProperty.h"
#include "FFloatProperty.h"
#include "FBoolProperty.h"
#include "FStrProperty.h"
#include "FObjectProperty.h"
#include "FArrayProperty.h"

#include <string>
#include <vector>
#include <type_traits>

template<typename T>
struct TIsArray
{
	static constexpr bool Value = false;
};

template<typename ElementType>
struct TIsArray<std::vector<ElementType>>
{
	static constexpr bool Value = true;
};

template<typename T>
FProperty* CreateProperty(const std::string& InName, const size_t InOffset)
{
	if constexpr (std::is_same_v<T, int>)
	{
		return new FIntProperty(InName, InOffset);
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		return new FFloatProperty(InName, InOffset);
	}
	else if constexpr (std::is_same_v<T, bool>)
	{
		return new FBoolProperty(InName, InOffset);
	}
	else if constexpr (std::is_same_v<T, std::string>)
	{
		return new FStrProperty(InName, InOffset);
	}
	else if constexpr (std::is_pointer_v<T> &&
						std::is_base_of_v<UObject, std::remove_pointer_t<T>>)
	{
		return new FObjectProperty(InName, InOffset);
	}
	else if constexpr (TIsArray<T>::Value)
	{
		using ElementType = typename T::value_type;
		static_assert(!TIsArray<ElementType>::Value, "CreateProperty : 이중 배열 방지");

		FProperty* Inner = CreateProperty<ElementType>(InName, 0);
		return new TArrayProperty<ElementType>(InName, InOffset, Inner);
	}
	else
	{
		static_assert(false, "CreateProperty : 지원하지 않는 타입");
	}
}
