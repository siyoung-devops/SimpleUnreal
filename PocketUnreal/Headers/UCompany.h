#pragma once

#include "ReflectionMacro.h"
#include "UObject.h"

#include <string>
#include <vector>

class UCompany : public UObject
{
public:
	GENERATED_BODY();

	UFUNCTION()
	void SetName(const std::string& InName);

	void AddCoworker(UObject* InCoworker);
	void AddSubsidiary(UObject* InSubsidiary);
	void RemoveSubsidiary(UObject* InSubsidiary);

private:
	UPROPERTY()
	std::string Name = "Overdare";

	UPROPERTY()
	std::vector<UObject*> Coworkers;

	UPROPERTY()
	std::vector<UObject*> Subsidiaries;
};
