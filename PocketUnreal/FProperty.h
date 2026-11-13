#pragma once
#include <vector>
#include <string>
#include "UObject.h"

// FProperty는 "멤버 변수 하나의 위치 정보"를 담는 객체
// FProperty 하나는 클래스의 멤버 변수 하나.
// UCoworker에 멤버가 세 개(Name, Age, Position)라면 FProperty도 세 개
// 오브젝트 주소를 받아서, 그 오브젝트 안에서 이 멤버가 있는 주소를 돌려주는 기능

class FProperty
{
public:
	FProperty(const std::string& InName, const size_t InOffset)
		: Name(InName)
		, Offset(InOffset)
	{
	}

	virtual ~FProperty() = default;

	const std::string& GetName() const { return Name; }
	size_t GetOffset() const { return Offset; }

	virtual std::string ExportText(const void* InContainer) const = 0;
	virtual void ImportText(void* InContainer, const std::string& InValue) const = 0;
	virtual std::string GetTypeName() const = 0;
	virtual void GetReferencedObjects(const void* InContainer, std::vector<UObject*>& OutObjects) const = 0;
	virtual bool HasObjectReference() const = 0;

private:
	std::string Name;
	size_t Offset = 0;
};