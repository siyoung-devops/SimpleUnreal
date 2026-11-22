#include "FObjectArray.h"

FObjectArray& FObjectArray::Get()
{
	static FObjectArray Instance;

	return Instance;
}

FObjectArray::FObjectArray()
{
	ObjectList.reserve(ReserveObjectCount);
}

int FObjectArray::GetObjectCount() const
{
	return static_cast<int>(ObjectList.size() - FreeIndexList.size());
}

int FObjectArray::AddObject(UObject* InObject)
{
	FObjectItem Item;
	Item.Object = InObject;

	int Index = 0;
	if (FreeIndexList.empty())
	{
		Index = static_cast<int>(ObjectList.size());
		ObjectList.push_back(Item);
	}
	else
	{
		Index = FreeIndexList.back();
		FreeIndexList.pop_back();
		ObjectList[Index] = Item;
	}

	return Index;
}

void FObjectArray::RemoveObject(const int InIndex)
{
	ObjectList[InIndex] = FObjectItem();
	FreeIndexList.push_back(InIndex);
}
