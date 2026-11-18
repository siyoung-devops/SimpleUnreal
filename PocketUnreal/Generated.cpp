#include "UCompany.h"
#include "UCoworker.h"
#include "TProperty.h"
#include "TFunction.h"

#include <cstddef>

const UClass* UCompany::StaticClass()
{
	static UClass ClassInstance = UClass("UCompany", Super::StaticClass(), sizeof(UCompany));
	static bool bRegistered = false;
	if (!bRegistered)
	{
		ClassInstance.AddProperty(new TProperty<decltype(UCompany::Coworkers)>("Coworkers", offsetof(ThisClass, Coworkers)));
		ClassInstance.AddProperty(new TProperty<decltype(UCompany::Subsidiaries)>("Subsidiaries", offsetof(ThisClass, Subsidiaries)));
		bRegistered = true;
	}
	return &ClassInstance;
}

const UClass* UCoworker::StaticClass()
{
	static UClass ClassInstance = UClass("UCoworker", Super::StaticClass(), sizeof(UCoworker));
	static bool bRegistered = false;
	if (!bRegistered)
	{
		ClassInstance.AddProperty(new TProperty<decltype(UCoworker::Name)>("Name", offsetof(ThisClass, Name)));
		ClassInstance.AddProperty(new TProperty<decltype(UCoworker::Age)>("Age", offsetof(ThisClass, Age)));
		ClassInstance.AddProperty(new TProperty<decltype(UCoworker::Position)>("Position", offsetof(ThisClass, Position)));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::SetAge)>("SetAge", &ThisClass::SetAge));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::GetAge)>("GetAge", &ThisClass::GetAge));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::SetName)>("SetName", &ThisClass::SetName));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::AddAge)>("AddAge", &ThisClass::AddAge));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::SetInfo)>("SetInfo", &ThisClass::SetInfo));
		bRegistered = true;
	}
	return &ClassInstance;
}
