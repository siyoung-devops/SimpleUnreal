#include "FGarbageCollector.h"
#include "UObject.h"
#include "UClass.h"
#include "FProperty.h"

#include <chrono>

void FGarbageCollector::MarkReference(UObject* InObject, std::vector<FObjectItem>& InObjectList, std::vector<UObject*>& OutWorkList)
{
	if (InObject == nullptr)
	{
		return;
	}

	FObjectItem& Item = InObjectList[InObject->GetInternalIndex()];
	if (!Item.bMarked)
	{
		Item.bMarked = true;
		OutWorkList.push_back(InObject);
	}
}

void FGarbageCollector::Mark()
{
	std::vector<UObject*> WorkList;
	std::vector<FObjectItem>& ObjectList = FObjectArray::Get().GetObjectList();

	WorkList.reserve(ObjectList.size());

	for (FObjectItem& Item : ObjectList)
	{
		if (Item.Object != nullptr && Item.bRooted)
		{
			Item.bMarked = true;
			WorkList.push_back(Item.Object);
		}
	}

	while (!WorkList.empty())
	{
		UObject* Current = WorkList.back();
		WorkList.pop_back();

		for (const UClass* Class = Current->GetClass(); Class != nullptr; Class = Class->GetSuperClass())
		{
			for (const FGCReferenceInfo& Info : Class->GetReferenceSchema())
			{
				const char* MemberAddress = reinterpret_cast<const char*>(Current) + Info.Offset;

				switch (Info.Kind)
				{
				case EGCReferenceKind::Object:
					{
						UObject* RefObject = *(reinterpret_cast<UObject* const*>(MemberAddress));
						MarkReference(RefObject, ObjectList, WorkList);
					}
					break;
				case EGCReferenceKind::ObjectArray:
					{
						const std::vector<UObject*>* RefObjectList = reinterpret_cast<const std::vector<UObject*>*>(MemberAddress);
						for (UObject* RefObject : *RefObjectList)
						{
							MarkReference(RefObject, ObjectList, WorkList);
						}
					}
					break;
				}
			}
		}
	}
}

void FGarbageCollector::Sweep()
{
	FObjectArray& ObjectArray = FObjectArray::Get();
	std::vector<FObjectItem>& ObjectList = ObjectArray.GetObjectList();

	for (int Index = 0; Index < static_cast<int>(ObjectList.size()); ++Index)
	{
		FObjectItem& Item = ObjectList[Index];
		if (Item.Object == nullptr)
		{
			continue;
		}

		if (Item.bMarked)
		{
			Item.bMarked = false;
		}
		else
		{
			delete Item.Object;
			ObjectArray.RemoveObject(Index);
		}
	}
}

double FGarbageCollector::GetTimeMs()
{
	std::chrono::steady_clock::duration Elapsed = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration<double, std::milli>(Elapsed).count();
}

FGCStats FGarbageCollector::Collect()
{
	FGCStats Stats;

	double StartMs = GetTimeMs();
	Mark();

	double MarkEndMs = GetTimeMs();
	Sweep();

	double SweepEndMs = GetTimeMs();

	Stats.MarkMs = MarkEndMs - StartMs;
	Stats.SweepMs = SweepEndMs - MarkEndMs;

	return Stats;
}
