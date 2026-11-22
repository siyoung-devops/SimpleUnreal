#include "UCompany.h"

void UCompany::SetName(const std::string& InName)
{
	if (InName.empty())
	{
		return;
	}

	Name = InName;
}

void UCompany::AddCoworker(UObject* InCoworker)
{
	Coworkers.push_back(InCoworker);
}

void UCompany::AddSubsidiary(UObject* InSubsidiary)
{
	Subsidiaries.push_back(InSubsidiary);
}

void UCompany::RemoveSubsidiary(UObject* InSubsidiary)
{
	size_t Index = 0;
	while (Index < Subsidiaries.size())
	{
		if (Subsidiaries[Index] == InSubsidiary)
		{
			Subsidiaries[Index] = Subsidiaries.back();
			Subsidiaries.pop_back();
			return;
		}

		++Index;
	}
}
