#pragma once

#include "UObject.h"

#include <vector>

struct FGCStats
{
	double MarkMs = 0.0;
	double SweepMs = 0.0;
};

class FGarbageCollector
{
public:
	static FGCStats Collect();

private:
	static void MarkReference(UObject* InObject, std::vector<FObjectItem>& InObjectList, std::vector<UObject*>& OutWorkList);
	static void Mark();
	static void Sweep();
	static double GetTimeMs();
};
