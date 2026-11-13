#pragma once

#include "ReflectionMacro.h"
#include "UObject.h"

#include <vector>

// GC가 회사 오브젝트만 보고도 "이 회사가 어떤 직원들을 가리키는지" 리플렉션으로 찾아낼 수 있게 만드는 거
class UCompany : public UObject
{
public:
	GENERATED_BODY(UCompany, UObject);

	void AddCoworker(UObject* InCoworker);
	void AddSubsidiary(UObject* InSubsidiary);

private:
	UPROPERTY(std::vector<UObject*>, Coworkers);
	UPROPERTY(std::vector<UObject*>, Subsidiaries);
};