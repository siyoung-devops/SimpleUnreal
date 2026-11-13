#pragma once
#include <vector>
#include <string>
#include "UObject.h"

// RPC는 함수 호출을 데이터로 바꿔서 전달하고,
// 받는 쪽이 그 데이터를 보고 함수를 찾아 실행하는 것.
// 문자열로 함수를 호출

// ① 보내는 쪽 : 호출을 데이터로. 함수 이름과 인자를 데이터로 만든다.	"SetAge 30" 문자열
// ② 전달	: 데이터를 상대에게 보낸다 (소켓, 패킷) > 이거까지는 구현할 필요 없음.
// ③ 받는 쪽 : 데이터를 호출로	. 이름으로 함수를 찾고, 인자를 풀어서 실행한다	FFunction / TFunction
// 받는 쪽은 "어떤 함수가 올지"를 미리 모르기 때문에, 이름으로 함수를 찾고 인자 타입에 맞게 값을 바꾸는 정보가 있어야 한다.

class FFunction
{
public:
	FFunction(const std::string& InName)
		: Name(InName)
	{
	}

	virtual ~FFunction() = default;

	const std::string& GetName() const { return Name; }

	// 함수 호출
	virtual std::string InvokeFromString(UObject* InObject, const std::string& InArgs) const = 0; 

private:
	std::string Name;
};
