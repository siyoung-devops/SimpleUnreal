#define NOMINMAX

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <windows.h>
#include <iostream>
#include <random>

#include "UClass.h"
#include "UObject.h"
#include "UCoworker.h"
#include "UCompany.h"
#include "FProperty.h"
#include "TFunction.h"
#include "FGarbageCollector.h"

constexpr int ObjectCount = 100000;
constexpr double LimitTime = 2.0;

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
		FGarbageCollector::IncrementalPurge(LimitTime, Stats);
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

UCompany* MakeCompany(bool bRoot)
{
	UCompany* Company = UObject::NewObject<UCompany>();

	if (bRoot)
	{
		Company->AddToRoot();
	}

	return Company;
}

void AddCoworkers(UCompany* Company, int Count)
{
	for (int i = 0; i < Count; ++i)
	{
		UCoworker* Coworker = UObject::NewObject<UCoworker>();
		Company->AddCoworker(Coworker);
	}
}

UCompany* ChainSubsidiaries(UCompany* Company, int Count)
{
	UCompany* LastCompany = nullptr;

	for (int i = 0; i < Count; ++i)
	{
		UCompany* ChildCompany = UObject::NewObject<UCompany>();
		Company->AddSubsidiary(ChildCompany);
		Company = LastCompany = ChildCompany;
	}

	return LastCompany;
}

std::vector<UCompany*> AddSubsidiaries(UCompany* Company, int Count)
{
	std::vector<UCompany*> Children;

	for (int i = 0; i < Count; ++i)
	{
		UCompany* ChildCompany = UObject::NewObject<UCompany>();
		Company->AddSubsidiary(ChildCompany);
		Children.push_back(ChildCompany);
	}

	return Children;
}

void FillCoworkers(UCompany* Company)
{
	int Count = ObjectCount - static_cast<int>(UObject::GetObjectList().size());
	AddCoworkers(Company, Count);
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
		FGarbageCollector::IncrementalPurge(LimitTime, Stats);
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

			for (int i = 0; i < ObjectCount - 2; ++i)
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
			// 모회사 → 자회사1 → 자회사2 → ... → 자회사 99,999
		    // 루트에서 계속 이어져있기 때문에 지워지지 않도록 함. 
			UCompany* ParentCompany = MakeCompany(true);
			ChainSubsidiaries(ParentCompany, ObjectCount - 1);
		}
		break;
	case EGCScenario::Hierarchy:
		{
			UCompany* ParentCompany = MakeCompany(true);
			int CompaniesNums = 10;
			int CoworkersNums = (ObjectCount - 111) / 110;
			// 모회사 → 자회사 1 직원 추가 → 손자회사 1..10 직원 추가
			//        → 자회사 2 직원 추가 → 손자회사 1..10
			std::vector<UCompany*> ChildrenCompanies = AddSubsidiaries(ParentCompany, CompaniesNums);
			for (UCompany* ChildCompany : ChildrenCompanies)
			{
				AddCoworkers(ChildCompany, CoworkersNums);
				
				std::vector<UCompany*> GrandChildren = AddSubsidiaries(ChildCompany, CompaniesNums);
				for (UCompany* GrandChild : GrandChildren)
				{
					AddCoworkers(GrandChild, CoworkersNums);
				}
			}

			// 루트에서 끊어서 자회사 삭제 
			ParentCompany->RemoveSubsidiary(ChildrenCompanies[0]);

			FillCoworkers(ParentCompany);
		}
		break;
	case EGCScenario::Cycle:
		{
			for(int i = 0; i < ObjectCount / 2; ++i)
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
