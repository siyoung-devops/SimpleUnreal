#include "UCompany.h"

void UCompany::AddCoworker(UObject* InCoworker)
{
	Coworkers.push_back(InCoworker);
}

void UCompany::AddSubsidiary(UObject* InSubsidiary)
{
	Subsidiaries.push_back(InSubsidiary);
}