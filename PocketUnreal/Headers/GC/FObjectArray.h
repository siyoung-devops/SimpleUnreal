#pragma once

#include <vector>

class UObject;

struct FObjectItem
{
	UObject* Object = nullptr;
	bool bRooted = false;
	bool bMarked = false;
};

class FObjectArray
{
public:
	static constexpr int ReserveObjectCount = 100000;

	static FObjectArray& Get();

	FObjectArray(const FObjectArray&) = delete;
	FObjectArray& operator=(const FObjectArray&) = delete;

	int AddObject(UObject* InObject);
	void RemoveObject(const int InIndex);

	FObjectItem& GetItem(const int InIndex) { return ObjectList[InIndex]; }
	std::vector<FObjectItem>& GetObjectList() { return ObjectList; }
	int GetObjectCount() const;

private:
	FObjectArray();

	std::vector<FObjectItem> ObjectList;
	std::vector<int> FreeIndexList;
};
