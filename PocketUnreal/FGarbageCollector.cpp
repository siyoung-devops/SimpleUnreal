#include <chrono>

#include "FGarbageCollector.h"
#include "UObject.h"
#include "UClass.h"
#include "FProperty.h"

void FGarbageCollector::Mark()
{
	std::vector<UObject*> WorkList;

	std::vector<UObject*> RefObjects;
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

		const std::vector<FProperty*>* ReferenceProperties = Current->GetClass()->GetReferenceProperties();
		for (FProperty* Property : *ReferenceProperties)
		{
			RefObjects.clear();
			Property->GetReferencedObjects(Current, RefObjects);

			for (UObject* RefObject : RefObjects)
			{
				if (!RefObject->IsMarked())
				{
					RefObject->Mark();
					WorkList.push_back(RefObject);
				}
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

static double GetTimeMs()
{
	std::chrono::steady_clock::duration Elapsed = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration<double, std::milli>(Elapsed).count();
}

// 누가 지워야할 오브젝트인지 골라내기
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

// Sweep이 골라낸, 지워야할 오브젝트 리스트 
std::vector<UObject*>& FGarbageCollector::GetPendingDeleteObjectList()
{
	static std::vector<UObject*> PendingDeleteObjectList;
	return PendingDeleteObjectList;
}

// 언리얼의 IncrementalPurgeGarbage도 오브젝트 하나마다 시계를 보지 않고, 일정 개수마다 확인
// 매번	10만 번 확인하면	시계 읽느라 삭제 시간이 거의 두 배가 될 수 있음
// 언리얼 방식대로 100개마다	1000번 으로 변경
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
		OutStats.PurgeMaxMs = ElapsedMs;
}