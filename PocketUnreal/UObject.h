#pragma once
#include <vector>
#include <string>

class UClass;

class UObject
{
public:
	virtual ~UObject() = default;
	UObject(const UObject&) = delete;
	UObject& operator=(const UObject&) = delete;

	const UClass* GetClass() const { return ClassPrivate; }
	static const UClass* StaticClass();
	static std::vector<UObject*>& GetObjectList();

	template <typename T>
	static T* NewObject()
	{
		T* Object = new T();

		Object->ClassPrivate = T::StaticClass();
		GetObjectList().push_back(Object);

		return Object;
	}

	void CallFunction(const std::string& InCommand);

	void AddToRoot() { bRooted = true; }
	void RemoveFromRoot() { bRooted = false; }
	bool IsRooted() const { return bRooted; }

	void Mark() { bMarked = true; }
	void UnMark() { bMarked = false; }
	bool IsMarked() const { return bMarked; }

protected:
	const UClass* ClassPrivate = nullptr;
	UObject() = default;

	bool bRooted = false;
	bool bMarked = false;
};