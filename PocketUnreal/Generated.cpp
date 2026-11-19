#include "UCompany.h"
#include "UCoworker.h"
#include "TProperty.h"
#include "TFunction.h"

#include <cstddef>

const UClass* UCompany::StaticClass()
{
	static UClass ClassInstance = UClass("UCompany", UObject::StaticClass());
	static bool bRegistered = false;
	if (!bRegistered)
	{
		ClassInstance.AddProperty(new TProperty<decltype(UCompany::Coworkers)>("Coworkers", offsetof(UCompany, Coworkers)));
		ClassInstance.AddProperty(new TProperty<decltype(UCompany::Subsidiaries)>("Subsidiaries", offsetof(UCompany, Subsidiaries)));
		bRegistered = true;
	}
	return &ClassInstance;
}

const UClass* UCoworker::StaticClass()
{
	static UClass ClassInstance = UClass("UCoworker", UObject::StaticClass());
	static bool bRegistered = false;
	if (!bRegistered)
	{
		ClassInstance.AddProperty(new TProperty<decltype(UCoworker::Name)>("Name", offsetof(UCoworker, Name)));
		ClassInstance.AddProperty(new TProperty<decltype(UCoworker::Age)>("Age", offsetof(UCoworker, Age)));
		ClassInstance.AddProperty(new TProperty<decltype(UCoworker::Position)>("Position", offsetof(UCoworker, Position)));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::SetAge)>("SetAge", &UCoworker::SetAge));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::GetAge)>("GetAge", &UCoworker::GetAge));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::SetName)>("SetName", &UCoworker::SetName));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::AddAge)>("AddAge", &UCoworker::AddAge));
		ClassInstance.AddFunction(new TFunction<decltype(&UCoworker::SetInfo)>("SetInfo", &UCoworker::SetInfo));
		bRegistered = true;
	}
	return &ClassInstance;
}
