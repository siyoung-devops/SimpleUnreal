#include "UClass.h"
#include "UObject.h"
#include "UCoworker.h"
#include "UCompany.h"
#include "FProperty.h"
#include "FGarbageCollector.h"

#include <iostream>
#include <random>
#include <Windows.h>

constexpr int ObjectCount = FObjectArray::ReserveObjectCount;

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

void MeasureGC(const char* InName)
{
	std::cout << "======== " << "[GC 시나리오] : " << InName << " ========" << std::endl;
	std::cout << "[GC 전 오브젝트 개수] : " << FObjectArray::Get().GetObjectCount() << std::endl;

	FGCStats Stats = FGarbageCollector::Collect();

	std::cout << "[GC 후 오브젝트 개수] : " << FObjectArray::Get().GetObjectCount() << std::endl;

	double MarkTime = Stats.MarkMs;
	double SweepTime = Stats.SweepMs;
	double TotalTime = MarkTime + SweepTime;

	std::cout << "[Mark 걸린 시간 ] : " << MarkTime << " ms" << std::endl;
	std::cout << "[Sweep 걸린 시간] : " << SweepTime << " ms" << std::endl;
	std::cout << "[   *GC 총합*   ] : " << TotalTime << " ms" << std::endl;
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
	int Count = ObjectCount - FObjectArray::Get().GetObjectCount();
	AddCoworkers(InCompany, Count);
}

void CleanUpAll()
{
	for (const FObjectItem& Item : FObjectArray::Get().GetItemList())
	{
		if (Item.Object != nullptr)
		{
			Item.Object->RemoveFromRoot();
		}
	}

	FGarbageCollector::Collect();
}

void PlayAllAlive()
{
	UCompany* Company = MakeCompany(true);
	FillCoworkers(Company);

	MeasureGC("AllAlive");
	CleanUpAll();
}

void PlayAllDead()
{
	UCompany* Company = MakeCompany(true);
	FillCoworkers(Company);
	Company->RemoveFromRoot();

	MeasureGC("AllDead");
	CleanUpAll();
}

void PlayRandom()
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

	MeasureGC("Random");
	CleanUpAll();
}

void PlayHalf()
{
	UCompany* AliveCompany = MakeCompany(true);
	UCompany* DeadCompany = MakeCompany(false);

	AddCoworkers(AliveCompany, (ObjectCount / 2) - 1);
	FillCoworkers(DeadCompany);

	MeasureGC("Half");
	CleanUpAll();
}

void PlayChain()
{
	UCompany* ParentCompany = MakeCompany(true);
	ChainSubsidiaries(ParentCompany, ObjectCount - 1);

	MeasureGC("Chain");
	CleanUpAll();
}

void PlayHierarchy()
{
	UCompany* ParentCompany = MakeCompany(true);

	constexpr int ChildCount = 10;
	constexpr int CoworkerCount = 900;

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

	MeasureGC("Hierarchy");
	CleanUpAll();
}

void PlayCycle()
{
	for (int Index = 0; Index < ObjectCount / 2; ++Index)
	{
		UCompany* CompanyA = MakeCompany(false);
		UCompany* CompanyB = MakeCompany(false);

		CompanyA->AddSubsidiary(CompanyB);
		CompanyB->AddSubsidiary(CompanyA);
	}

	MeasureGC("Cycle");
	CleanUpAll();
}

void PlayRing()
{
	UCompany* ParentCompany = MakeCompany(true);
	UCompany* LastCompany = ChainSubsidiaries(ParentCompany, ObjectCount - 1);

	LastCompany->AddSubsidiary(ParentCompany);

	MeasureGC("Ring");
	CleanUpAll();
}

void PlayReflection()
{
	UCompany* Company = MakeCompany(true);
	UCoworker* Coworker = UObject::NewObject<UCoworker>();
	Company->AddCoworker(Coworker);

	PrintProperties(Company);

	Coworker->CallFunction("SetInfo Lee 30");
	Coworker->CallFunction("AddAge 5");
	Coworker->CallFunction("SetHeight 180.5");
	Coworker->CallFunction("SetWorking false");
	PrintProperties(Coworker);

	CleanUpAll();
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	PlayAllAlive();
	PlayAllDead();
	PlayRandom();
	PlayHalf();
	PlayChain();
	PlayHierarchy();
	PlayCycle();
	PlayRing();
	PlayReflection();

	return 0;
}
