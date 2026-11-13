#define NOMINMAX
#include <windows.h>

#include <iostream>

#include "UClass.h"
#include "UObject.h"
#include "UCoworker.h"
#include "UCompany.h"
#include "FProperty.h"
#include "TFunction.h"
#include "FGarbageCollector.h"

#include <chrono>
#include <iomanip>


int main()
{
	SetConsoleOutputCP(CP_UTF8);

	UObject* pCompany = UObject::NewObject<UCompany>();

	// 루트 지정 후 GC를 이용해 Mark안된거 삭제
	pCompany->AddToRoot();

	for (int i = 0; i < 100000; ++i)
	{
		UObject* pObject = UObject::NewObject<UCoworker>();
		dynamic_cast<UCompany*>(pCompany)->AddCoworker(pObject);
	}

	std::cout << pCompany->GetObjectList().size() << std::endl;
	auto start = std::chrono::steady_clock::now();

	pCompany->RemoveFromRoot();
	FGarbageCollector::Collect();

	auto end = std::chrono::steady_clock::now();
	std::cout << pCompany->GetObjectList().size() << std::endl;

	double gcTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
	std::cout << "GC: " << gcTimeMs << " ms" << std::endl;


	return 0;
}