#include "UCompany.h"
#include "UCoworker.h"
#include "CreateProperty.h"
#include "TFunction.h"

#include <cstddef>

const UClass* UCompany::StaticClass()
{
	static UClass ClassInstance = UClass("UCompany", UObject::StaticClass());
	static bool bRegistered = false;
	if (!bRegistered)
	{
		ClassInstance.AddProperty(CreateProperty<decltype(UCompany::Name)>("Name", offsetof(UCompany, Name)));
		ClassInstance.AddProperty(CreateProperty<decltype(UCompany::Coworkers)>("Coworkers", offsetof(UCompany, Coworkers)));
		ClassInstance.AddProperty(CreateProperty<decltype(UCompany::Subsidiaries)>("Subsidiaries", offsetof(UCompany, Subsidiaries)));
		ClassInstance.AddFunction(CreateFunction("SetName", &UCompany::SetName));
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
		ClassInstance.AddProperty(CreateProperty<decltype(UCoworker::Name)>("Name", offsetof(UCoworker, Name)));
		ClassInstance.AddProperty(CreateProperty<decltype(UCoworker::Age)>("Age", offsetof(UCoworker, Age)));
		ClassInstance.AddProperty(CreateProperty<decltype(UCoworker::Height)>("Height", offsetof(UCoworker, Height)));
		ClassInstance.AddProperty(CreateProperty<decltype(UCoworker::bWorking)>("bWorking", offsetof(UCoworker, bWorking)));
		ClassInstance.AddFunction(CreateFunction("SetAge", &UCoworker::SetAge));
		ClassInstance.AddFunction(CreateFunction("GetAge", &UCoworker::GetAge));
		ClassInstance.AddFunction(CreateFunction("SetName", &UCoworker::SetName));
		ClassInstance.AddFunction(CreateFunction("AddAge", &UCoworker::AddAge));
		ClassInstance.AddFunction(CreateFunction("SetInfo", &UCoworker::SetInfo));
		ClassInstance.AddFunction(CreateFunction("SetHeight", &UCoworker::SetHeight));
		ClassInstance.AddFunction(CreateFunction("SetWorking", &UCoworker::SetWorking));
		bRegistered = true;
	}
	return &ClassInstance;
}
