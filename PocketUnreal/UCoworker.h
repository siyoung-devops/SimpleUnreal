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
	GENERATED_BODY(UCoworker, UObject);

	UFUNCTION(SetAge);
	UFUNCTION(GetAge);
	UFUNCTION(SetName);
	UFUNCTION(AddAge);
	UFUNCTION(SetInfo);

	void SetAge(const int InAge);
	int GetAge() const;
	void SetName(const std::string& InName);
	int AddAge(const int InAge);
	void SetInfo(const std::string& InName, const int InAge);

private:
	UPROPERTY(std::string, Name) = "Henry";
	UPROPERTY(int, Age) = 40;
	UPROPERTY(ECoworkerPosition, Position) = ECoworkerPosition::Boss;
};