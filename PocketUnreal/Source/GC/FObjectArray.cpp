#include "FObjectArray.h"

FObjectArray& FObjectArray::Get()
{
	static FObjectArray Instance;

	return Instance;
}

FObjectArray::FObjectArray()
{
	ItemList.reserve(ReserveObjectCount);
	FreeIndexList.reserve(ReserveObjectCount);
}

int FObjectArray::GetObjectCount() const
{
	return static_cast<int>(ItemList.size() - FreeIndexList.size());
}

int FObjectArray::AddObject(UObject* InObject)
{
	FObjectItem Item;
	Item.Object = InObject;

	int Index = 0;
	if (FreeIndexList.empty())
	{
		Index = static_cast<int>(ItemList.size());
		ItemList.push_back(Item);
	}
	else
	{
		Index = FreeIndexList.back();
		FreeIndexList.pop_back();
		ItemList[Index] = Item;
	}

	return Index;
}

void FObjectArray::RemoveObject(const int InIndex)
{
	ItemList[InIndex] = FObjectItem();
	FreeIndexList.push_back(InIndex);
}
