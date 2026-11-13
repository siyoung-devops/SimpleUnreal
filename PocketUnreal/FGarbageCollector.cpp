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
	std::vector<UObject*> AliveList;
	std::vector<UObject*>& ObjectList = UObject::GetObjectList();
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
			delete Current;
		}
	}

	ObjectList.swap(AliveList);
}

static double GetTimeMs()
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