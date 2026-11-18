#define NOMINMAX

#include "UClass.h"
#include "UObject.h"
#include "UCoworker.h"
#include "UCompany.h"
#include "FProperty.h"
#include "TFunction.h"
#include "FGarbageCollector.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <windows.h>

constexpr int ObjectCount = 100000;
constexpr double PurgeTimeLimitMs = 2.0;

enum class EGCScenario
{
	AllAlive,
	AllDead,
	Random,
	Half,
	Chain,
	Hierarchy,
	Cycle,
	Ring
};

const char* ToString(const EGCScenario InScenario)
{
	switch (InScenario)
	{
	case EGCScenario::AllAlive:
		return "AllAlive";
	case EGCScenario::AllDead:
		return "AllDead";
	case EGCScenario::Random:
		return "Random";
	case EGCScenario::Half:
		return "Half";
	case EGCScenario::Chain:
		return "Chain";
	case EGCScenario::Hierarchy:
		return "Hierarchy";
	case EGCScenario::Cycle:
		return "Cycle";
	case EGCScenario::Ring:
		return "Ring";
	}
	return "Unknown";
}

void PrintProperties(const UObject* InObject)
{
	std::cout << "======================================================" << std::endl;
	for (const UClass* Class = InObject->GetClass(); Class != nullptr; Class = Class->GetSuperClass())
	{
		std::cout << "[" << Class->GetName() << "]" << std::endl;
		const std::vector<FProperty*>& Properties = Class->GetProperties();
		for (FProperty* Property : Properties)
		{
			std::cout << Property->GetName() << "(Type : " << Property->GetTypeName() <<
				") : " << Property->ExportText(InObject) << std::endl;
		}
	}
}

void MeasureGC(const EGCScenario InScenario)
{
	std::cout << "======== " << "[GC 시나리오] : " << ToString(InScenario) << " ========" << std::endl;
	std::cout << "[GC 전 오브젝트 개수] : " << UObject::GetObjectList().size() << std::endl;

	FGCStats Stats = FGarbageCollector::Collect();

	while (FGarbageCollector::IsPurgePending())
	{
		FGarbageCollector::IncrementalPurge(PurgeTimeLimitMs, Stats);
	}

	std::cout << "[GC 후 오브젝트 개수] : " << UObject::GetObjectList().size() << std::endl;

	double MarkTime = Stats.MarkMs;
	double SweepTime = Stats.SweepMs;
	double CollectTime = MarkTime + SweepTime;
	double TotalTime = CollectTime + Stats.PurgeTotalMs;
	double MaxPauseTime = std::max(CollectTime, Stats.PurgeMaxMs);

	std::cout << "[   수집 시간   ] : " << CollectTime << " ms" << std::endl;
	std::cout << "[Mark 걸린 시간 ] : " << MarkTime << " ms" << std::endl;
	std::cout << "[Sweep 걸린 시간] : " << SweepTime << " ms" << std::endl;
	std::cout << "[   삭제 횟수   ] : " << Stats.PurgeCount << " 회" << std::endl;
	std::cout << "[ 삭제 1회 최대 ] : " << Stats.PurgeMaxMs << " ms" << std::endl;
	std::cout << "[   삭제 총합   ] : " << Stats.PurgeTotalMs << " ms" << std::endl;
	std::cout << "[   *GC 총합*   ] : " << TotalTime << " ms" << std::endl;
	std::cout << "[   최대 멈춤   ] : " << MaxPauseTime << " ms" << std::endl;
}

UCompany* MakeCompany(const bool bInRoot)
{
	UCompany* Company = UObject::NewObject<UCompany>();

	if (bInRoot)
	{
		Company->AddToRoot();
	}

	return Company;
}

void AddCoworkers(UCompany* InCompany, const int InCount)
{
	for (int Index = 0; Index < InCount; ++Index)
	{
		UCoworker* Coworker = UObject::NewObject<UCoworker>();
		InCompany->AddCoworker(Coworker);
	}
}

UCompany* ChainSubsidiaries(UCompany* InCompany, const int InCount)
{
	UCompany* CurrentCompany = InCompany;

	for (int Index = 0; Index < InCount; ++Index)
	{
		UCompany* ChildCompany = UObject::NewObject<UCompany>();
		CurrentCompany->AddSubsidiary(ChildCompany);
		CurrentCompany = ChildCompany;
	}

	return CurrentCompany;
}

std::vector<UCompany*> AddSubsidiaries(UCompany* InCompany, const int InCount)
{
	std::vector<UCompany*> ChildCompanies;

	for (int Index = 0; Index < InCount; ++Index)
	{
		UCompany* ChildCompany = UObject::NewObject<UCompany>();
		InCompany->AddSubsidiary(ChildCompany);
		ChildCompanies.push_back(ChildCompany);
	}

	return ChildCompanies;
}

void FillCoworkers(UCompany* InCompany)
{
	int Count = ObjectCount - static_cast<int>(UObject::GetObjectList().size());
	AddCoworkers(InCompany, Count);
}

void CleanUpAll()
{
	for (UObject* Object : UObject::GetObjectList())
	{
		Object->RemoveFromRoot();
	}

	FGCStats Stats = FGarbageCollector::Collect();
	while (FGarbageCollector::IsPurgePending())
	{
		FGarbageCollector::IncrementalPurge(PurgeTimeLimitMs, Stats);
	}
}

void PlayGCScenario(const EGCScenario InScenario)
{
	switch (InScenario)
	{
	case EGCScenario::AllAlive:
		{
			UCompany* Company = MakeCompany(true);
			FillCoworkers(Company);
		}
		break;
	case EGCScenario::AllDead:
		{
			UCompany* Company = MakeCompany(true);
			FillCoworkers(Company);
			Company->RemoveFromRoot();
		}
		break;
	case EGCScenario::Random:
		{
			UCompany* AliveCompany = MakeCompany(true);
			UCompany* DeadCompany = MakeCompany(false);

			std::mt19937 Random(std::random_device{}());
			std::uniform_int_distribution<int> Coin(0, 1);

			for (int Index = 0; Index < ObjectCount - 2; ++Index)
			{
				if (Coin(Random) == 1)
				{
					AddCoworkers(AliveCompany, 1);
				}
				else
				{
					AddCoworkers(DeadCompany, 1);
				}
			}
		}
		break;
	case EGCScenario::Half:
		{
			UCompany* AliveCompany = MakeCompany(true);
			UCompany* DeadCompany = MakeCompany(false);

			AddCoworkers(AliveCompany, (ObjectCount / 2) - 1);
			FillCoworkers(DeadCompany);
		}
		break;
	case EGCScenario::Chain:
		{
			UCompany* ParentCompany = MakeCompany(true);
			ChainSubsidiaries(ParentCompany, ObjectCount - 1);
		}
		break;
	case EGCScenario::Hierarchy:
		{
			UCompany* ParentCompany = MakeCompany(true);

			int ChildCount = 10;
			int CoworkerCount = 900;

			std::vector<UCompany*> ChildCompanies = AddSubsidiaries(ParentCompany, ChildCount);
			for (UCompany* ChildCompany : ChildCompanies)
			{
				AddCoworkers(ChildCompany, CoworkerCount);

				std::vector<UCompany*> GrandChildCompanies = AddSubsidiaries(ChildCompany, ChildCount);
				for (UCompany* GrandChildCompany : GrandChildCompanies)
				{
					AddCoworkers(GrandChildCompany, CoworkerCount);
				}
			}

			ParentCompany->RemoveSubsidiary(ChildCompanies[0]);

			FillCoworkers(ParentCompany);
		}
		break;
	case EGCScenario::Cycle:
		{
			for (int Index = 0; Index < ObjectCount / 2; ++Index)
			{
				UCompany* CompanyA = MakeCompany(false);
				UCompany* CompanyB = MakeCompany(false);

				CompanyA->AddSubsidiary(CompanyB);
				CompanyB->AddSubsidiary(CompanyA);
			}
		}
		break;
	case EGCScenario::Ring:
		{
			UCompany* ParentCompany = MakeCompany(true);
			UCompany* LastCompany = ChainSubsidiaries(ParentCompany, ObjectCount - 1);

			LastCompany->AddSubsidiary(ParentCompany);
		}
		break;
	default:
		break;
	}

	MeasureGC(InScenario);
	CleanUpAll();
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	PlayGCScenario(EGCScenario::AllAlive);
	PlayGCScenario(EGCScenario::AllDead);
	PlayGCScenario(EGCScenario::Random);
	PlayGCScenario(EGCScenario::Half);
	PlayGCScenario(EGCScenario::Chain);
	PlayGCScenario(EGCScenario::Hierarchy);
	PlayGCScenario(EGCScenario::Cycle);
	PlayGCScenario(EGCScenario::Ring);

	UCoworker* Coworker = UObject::NewObject<UCoworker>();
	PrintProperties(Coworker);
	Coworker->CallFunction("SetInfo Lee 30");
	PrintProperties(Coworker);
	std::cout << "======================================================" << std::endl;
	Coworker->CallFunction("GetAge");

	CleanUpAll();

	return 0;
}
