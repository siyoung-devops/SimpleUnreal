#include "FArrayProperty.h"

#include <stdexcept>

FArrayProperty::~FArrayProperty()
{
	delete Inner;
}

std::string FArrayProperty::ExportText(const void* InContainer) const
{
	std::string Text = "[";

	const size_t Num = GetNum(InContainer);
	for (size_t Index = 0; Index < Num; ++Index)
	{
		if (Index > 0)
		{
			Text += ", ";
		}

		Text += Inner->ExportText(GetElementPtr(InContainer, Index));
	}

	Text += "]";
	return Text;
}

void FArrayProperty::ImportText(void*, const std::string&) const
{
	throw std::invalid_argument("지원하지 않는 인자 타입");
}

std::string FArrayProperty::GetTypeName() const
{
	return "Array<" + Inner->GetTypeName() + ">";
}

EGCReferenceKind FArrayProperty::GetReferenceKind() const
{
	if (Inner->GetReferenceKind() == EGCReferenceKind::Object)
	{
		return EGCReferenceKind::ObjectArray;
	}

	return EGCReferenceKind::None;
}
