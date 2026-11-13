#define NOMINMAX

#include <chrono>
#include <iomanip>
#include <windows.h>
#include <iostream>

#include "UClass.h"
#include "UObject.h"
#include "UCoworker.h"
#include "UCompany.h"
#include "FProperty.h"
#include "TFunction.h"
#include "FGarbageCollector.h"

constexpr int ObjectCount = 100000;

enum class EGCScenario
{
	AllAlive,
	AllDead,
	Random, //
	Half	//
};

void MeasureGC()
{
	std::cout << "[오브젝트 개수] : " << UObject::GetObjectList().size() << std::endl;
	auto start = std::chrono::steady_clock::now();

	FGarbageCollector::Collect();

	auto end = std::chrono::steady_clock::now();
	std::cout << "[오브젝트 개수] : " << UObject::GetObjectList().size() << std::endl;

	double gcTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
	std::cout << "[GC 걸린 시간] : " << gcTimeMs << " ms" << std::endl;
}

// 회사 1개(루트) + 직원 99,999명 추가
UCompany* BuildCompany()
{
	UCompany* Company = UObject::NewObject<UCompany>();
	Company->AddToRoot();

	for (int i = 0; i < ObjectCount - 1; ++i)
	{
		UCoworker* Coworker = UObject::NewObject<UCoworker>();
		Company->AddCoworker(Coworker);
	}

	return Company;
}

void CleanUpAll()
{
	for (UObject* Object : UObject::GetObjectList())
	{
		Object->RemoveFromRoot();
	}

	FGarbageCollector::Collect();
}

void PlayGCScenario(const EGCScenario InScenario)
{
	switch (InScenario)
	{
	case EGCScenario::AllAlive:
		{
			BuildCompany();
		}
		break;
	case EGCScenario::AllDead:
		{
			UCompany* Company = BuildCompany();
			Company->RemoveFromRoot();
		}
		break;
	case EGCScenario::Random:
		{

		}
		break;
	case EGCScenario::Half:
		{

		}
		break;
	default:
		break;
	}

	MeasureGC();
	CleanUpAll();
}


int main()
{
	SetConsoleOutputCP(CP_UTF8);

	PlayGCScenario(EGCScenario::AllAlive);
	PlayGCScenario(EGCScenario::AllDead);

	return 0;
}
