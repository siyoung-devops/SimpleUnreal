#include "FObjectProperty.h"
#include "UClass.h"

#include <stdexcept>

std::string FObjectProperty::ExportText(const void* InContainer) const
{
	UObject* Object = *static_cast<UObject* const*>(GetValuePtr(InContainer));
	if (Object == nullptr)
	{
		return "None";
	}

	return Object->GetClass()->GetName();
}

void FObjectProperty::ImportText(void*, const std::string&) const
{
	throw std::invalid_argument("지원하지 않는 인자 타입");
}

std::string FObjectProperty::GetTypeName() const
{
	return "Object";
}

EGCReferenceKind FObjectProperty::GetReferenceKind() const
{
	return EGCReferenceKind::Object;
}
