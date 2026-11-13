#pragma once

#include <vector>
#include "UObject.h"

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
	static void Mark();
	static void Sweep();
};