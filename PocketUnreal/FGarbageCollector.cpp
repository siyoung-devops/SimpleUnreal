#include <chrono>

#include "FGarbageCollector.h"
#include "UObject.h"
#include "UClass.h"
#include "FProperty.h"


void FGarbageCollector::MarkReference(UObject* InObject, std::vector<UObject*>& OutWorkList)
{
	if (InObject == nullptr)
	{
		return;
	}

	if (!InObject->IsMarked())
	{
		InObject->Mark();
		OutWorkList.push_back(InObject);
	}
}

void FGarbageCollector::Mark()
{
	std::vector<UObject*> WorkList;
	std::vector<UObject*>& ObjectList = UObject::GetObjectList();

	WorkList.reserve(ObjectList.size());

	for (UObject* Object : ObjectList)
	{
		if (Object->IsRooted())
		{
			Object->Mark();
			WorkList.push_back(Object);
		}
	}

	while (!WorkList.empty())
	{
		UObject* Current = WorkList.back();
		WorkList.pop_back();

		for (const FGCReferenceInfo& Info : Current->GetClass()->GetReferenceSchema())
		{
			const char* MemberAddress = reinterpret_cast<const char*>(Current) + Info.Offset;

			switch (Info.Kind)
			{
			case EGCReferenceKind::Object:
				{
					UObject* RefObject = *(reinterpret_cast<UObject* const*>(MemberAddress));
					MarkReference(RefObject, WorkList);
				}
				break;
			case EGCReferenceKind::ObjectArray:
				{
					const std::vector<UObject*>* RefObjectList = reinterpret_cast<const std::vector<UObject*>*>(MemberAddress);
					for (UObject* RefObject : *RefObjectList)
					{
						MarkReference(RefObject, WorkList);
					}
				}
				break;
			}
		}
	}
}

void FGarbageCollector::Sweep()
{
	std::vector<UObject*>& ObjectList = UObject::GetObjectList();
	std::vector<UObject*>& PendingDeleteObjectList = GetPendingDeleteObjectList();

	std::vector<UObject*> AliveList;
	AliveList.reserve(ObjectList.size());

	while (!ObjectList.empty())
	{
		UObject* Current = ObjectList.back();
		ObjectList.pop_back();

		if (Current->IsMarked())
		{
			Current->UnMark();
			AliveList.push_back(Current);
		}
		else
		{
			PendingDeleteObjectList.push_back(Current);
		}
	}

	ObjectList.swap(AliveList);
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

std::vector<UObject*>& FGarbageCollector::GetPendingDeleteObjectList()
{
	static std::vector<UObject*> PendingDeleteObjectList;
	return PendingDeleteObjectList;
}

void FGarbageCollector::IncrementalPurge(double InTimeLimitMs, FGCStats& OutStats)
{
	double StartMs = GetTimeMs();
	int DeleteCount = 0;

	std::vector<UObject*>& PendingDeleteObjectList = GetPendingDeleteObjectList();

	while(!PendingDeleteObjectList.empty())
	{
		UObject* Current = PendingDeleteObjectList.back();
		PendingDeleteObjectList.pop_back();

		delete Current;

		++DeleteCount;

		if ((DeleteCount % 100) == 0 && (GetTimeMs() - StartMs) > InTimeLimitMs)
			break;
	}

	double ElapsedMs = GetTimeMs() - StartMs;
	++OutStats.PurgeCount;
	OutStats.PurgeTotalMs += ElapsedMs;
	if (ElapsedMs > OutStats.PurgeMaxMs)
	{
		OutStats.PurgeMaxMs = ElapsedMs;
	}
}