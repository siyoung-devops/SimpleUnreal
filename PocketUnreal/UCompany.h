#pragma once

#include "ReflectionMacro.h"
#include "UObject.h"

#include <vector>

class UCompany : public UObject
{
public:
	GENERATED_BODY();

	void AddCoworker(UObject* InCoworker);
	void AddSubsidiary(UObject* InSubsidiary);
	void RemoveSubsidiary(UObject* InSubsidiary);

private:
	UPROPERTY()
	std::vector<UObject*> Coworkers;

	UPROPERTY()
	std::vector<UObject*> Subsidiaries;
};
