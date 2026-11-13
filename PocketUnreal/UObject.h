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

	// RPC
	void CallFunction(const std::string& InCommand);

	// GC - 루트 지정
	void AddToRoot() { bRooted = true; }
	void RemoveFromRoot() { bRooted = false; }
	bool IsRooted() const { return bRooted; }

	// GC - 마크, 루트에서 따라가서 닿은 오브젝트들. 아직 쓰고 있는 것들
	//      아무도 가리키지 않는 애들을 Sweep에서 delete
	void Mark() { bMarked = true; }
	void UnMark() { bMarked = false; }
	bool IsMarked() const { return bMarked; }

protected:
	const UClass* ClassPrivate = nullptr;
	UObject() = default;

	bool bRooted = false;
	bool bMarked = false;
};