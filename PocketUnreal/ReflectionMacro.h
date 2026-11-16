#pragma once
#include "TProperty.h"
#include "TFunction.h"
#include "UClass.h"

#include <cstddef>
#include <string>
#include <vector>

struct FRegisterFunction
{
	FRegisterFunction(std::vector<void(*)(UClass&)>& InFunctions, void(*InFunction)(UClass&))
	{
		InFunctions.push_back(InFunction);
	}
};

#define GENERATED_BODY(Class, SuperClass) \
public: \
	using ThisClass = Class; \
	using Super = SuperClass; \
	static std::vector<void(*)(UClass&)>& GetRegisterFunctions() \
	{ \
		static std::vector<void(*)(UClass&)> Functions; \
		return Functions; \
	} \
	static const UClass* StaticClass() \
	{ \
		static UClass ClassInstance = UClass(\
			#Class, Super::StaticClass(), sizeof(ThisClass) \
		); \
		static bool bRegistered = false; \
		if (!bRegistered) \
		{ \
			for (void(*Function)(UClass&) : GetRegisterFunctions()) \
			{ \
				Function(ClassInstance); \
			} \
			bRegistered = true; \
		} \
		return &ClassInstance; \
	}


#define UPROPERTY(Type, Name) \
	static void RegisterProperty_##Name(UClass& InClass) \
	{ \
		InClass.AddProperty(new TProperty<Type>(#Name, offsetof(ThisClass, Name))); \
	} \
	inline static FRegisterFunction PropertyRegistrar_##Name \
	{ \
		GetRegisterFunctions(), &RegisterProperty_##Name \
	}; \
	Type Name


#define UFUNCTION(Name) \
	static void RegisterFunction_##Name(UClass& InClass) \
	{ \
		InClass.AddFunction(new TFunction<decltype(&ThisClass::Name)>(#Name, &ThisClass::Name)); \
	} \
	inline static FRegisterFunction FunctionRegistrar_##Name \
	{ \
		GetRegisterFunctions(), &RegisterFunction_##Name \
	};
