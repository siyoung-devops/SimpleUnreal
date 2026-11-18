#pragma once

#include "FProperty.h"

#include <stdexcept>
#include <type_traits>

template<typename T>
struct TIsVector : std::false_type {};

template<typename ElementType>
struct TIsVector<std::vector<ElementType>> : std::true_type {};

template<typename T>
constexpr bool TIsObjectPointer = std::is_pointer_v<T> && std::is_base_of_v<UObject, std::remove_pointer_t<T>>;

template<typename T>
class TProperty : public FProperty
{
public:
	TProperty(const std::string& InName, const size_t InOffset)
		: FProperty(InName, InOffset)
	{
	}

	std::string ExportText(const void* InContainer) const override
	{
		if (InContainer == nullptr)
		{
			return "None";
		}

		const T* ValuePtr = reinterpret_cast<const T*>(reinterpret_cast<const char*>(InContainer) + GetOffset());

		if constexpr (std::is_same_v<T, int>)
		{
			return std::to_string(*ValuePtr);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return *ValuePtr;
		}
		else if constexpr (std::is_enum_v<T>)
		{
			return std::to_string(static_cast<int>(*ValuePtr));
		}
		else if constexpr (TIsObjectPointer<T>)
		{
			if (*ValuePtr == nullptr)
			{
				return "None";
			}

			return (*ValuePtr)->GetClass()->GetName();
		}
		else if constexpr (TIsVector<T>::value)
		{
			std::string Text = "[";
			static TProperty<typename T::value_type> ElementProperty("", 0);

			for (const auto& Element : *ValuePtr)
			{
				if (Text.size() > 1)
				{
					Text += ", ";
				}

				Text += ElementProperty.ExportText(&Element);
			}

			Text += "]";
			return Text;
		}
		else
		{
			static_assert(sizeof(T) == 0, "TProperty: 지원하지 않는 타입");
		}
	}

	void ImportText(void* InContainer, const std::string& InValue) const override
	{
		if (InContainer == nullptr)
		{
			return;
		}

		T* ValuePtr = reinterpret_cast<T*>(reinterpret_cast<char*>(InContainer) + GetOffset());

		if constexpr (std::is_same_v<T, int>)
		{
			if (InValue.empty())
			{
				return;
			}

			const T NewValue = std::stoi(InValue);
			*ValuePtr = NewValue;
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			*ValuePtr = InValue;
		}
		else if constexpr (std::is_enum_v<T>)
		{
			if (InValue.empty())
			{
				return;
			}

			const int NewValue = std::stoi(InValue);
			*ValuePtr = static_cast<T>(NewValue);
		}
		else if constexpr (TIsObjectPointer<T> || TIsVector<T>::value)
		{
			throw std::invalid_argument("지원하지 않는 인자 타입");
		}
		else
		{
			static_assert(sizeof(T) == 0, "TProperty: 지원하지 않는 타입");
		}
	}

	std::string GetTypeName() const override
	{
		if constexpr (std::is_same_v<T, int>)
		{
			return "int";
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return "string";
		}
		else if constexpr (std::is_enum_v<T>)
		{
			return "enum";
		}
		else if constexpr (TIsObjectPointer<T>)
		{
			return "Object";
		}
		else if constexpr (TIsVector<T>::value)
		{
			static TProperty<typename T::value_type> ElementProperty("", 0);
			return "Array<" + ElementProperty.GetTypeName() + ">";
		}
		else
		{
			static_assert(sizeof(T) == 0, "TProperty: 지원하지 않는 타입");
		}
	}

	EGCReferenceKind GetReferenceKind() const override
	{
		if constexpr (TIsObjectPointer<T>)
		{
			return EGCReferenceKind::Object;
		}
		else if constexpr (TIsVector<T>::value)
		{
			static_assert(!TIsVector<typename T::value_type>::value, "TProperty: 중첩 배열 방지");

			if constexpr (TIsObjectPointer<typename T::value_type>)
			{
				return EGCReferenceKind::ObjectArray;
			}
		}
		return EGCReferenceKind::None;
	}
};
