#pragma once

#include "UObject.h"

#include <vector>

struct FGCStats
{
	double MarkMs = 0.0;
	double SweepMs = 0.0;
	int PurgeCount = 0;
	double PurgeMaxMs = 0.0;
	double PurgeTotalMs = 0.0;
};

class FGarbageCollector
{
public:
	static FGCStats Collect();
	static bool IsPurgePending() { return !GetPendingDeleteObjectList().empty(); }
	static void IncrementalPurge(double InTimeLimitMs, FGCStats& OutStats);

private:
	static void MarkReference(UObject* InObject, std::vector<UObject*>& OutWorkList);
	static void Mark();
	static void Sweep();
	static double GetTimeMs();

	static std::vector<UObject*>& GetPendingDeleteObjectList();
};
