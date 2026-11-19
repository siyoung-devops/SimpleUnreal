#pragma once

#include "ReflectionMacro.h"
#include "UObject.h"

#include <string>
#include <vector>

enum class ECoworkerPosition
{
	Staff,
	Manager,
	Boss
};

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

private:
	UPROPERTY()
	std::string Name = "Henry";

	UPROPERTY()
	int Age = 40;

	UPROPERTY()
	ECoworkerPosition Position = ECoworkerPosition::Boss;
};
