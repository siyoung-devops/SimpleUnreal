#pragma once

#include "ReflectionMacro.h"
#include "UObject.h"

#include <string>

class UCoworker : public UObject
{
public:
	GENERATED_BODY();

	UFUNCTION()
	void SetAge(const int InAge);

	UFUNCTION()
	int GetAge() const;

	UFUNCTION()
	void SetName(const std::string& InName);

	UFUNCTION()
	int AddAge(const int InAge);

	UFUNCTION()
	void SetInfo(const std::string& InName, const int InAge);

	UFUNCTION()
	void SetHeight(const float InHeight);

	UFUNCTION()
	void SetWorking(const bool bInWorking);

private:
	UPROPERTY()
	std::string Name = "Lee";

	UPROPERTY()
	int Age = 40;

	UPROPERTY()
	float Height = 175.5f;

	UPROPERTY()
	bool bWorking = true;
};
