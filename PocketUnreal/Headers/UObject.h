#pragma once

#include "FObjectArray.h"

#include <string>

class UClass;

class UObject
{
public:
	UObject() = default;
	virtual ~UObject() = default;
	UObject(const UObject&) = delete;
	UObject& operator=(const UObject&) = delete;

	template<typename T>
	static T* NewObject()
	{
		T* Object = new T();
		Object->ClassPrivate = T::StaticClass();
		Object->InternalIndex = FObjectArray::Get().AddObject(Object);

		return Object;
	}

	static const UClass* StaticClass();

	const UClass* GetClass() const { return ClassPrivate; }
	int GetInternalIndex() const { return InternalIndex; }

	void CallFunction(const std::string& InCommand);

	void AddToRoot() { FObjectArray::Get().GetItem(InternalIndex).bRooted = true; }
	void RemoveFromRoot() { FObjectArray::Get().GetItem(InternalIndex).bRooted = false; }
	bool IsRooted() const { return FObjectArray::Get().GetItem(InternalIndex).bRooted; }

private:
	const UClass* ClassPrivate = nullptr;
	int InternalIndex = -1;
};
