#include "UCoworker.h"

void UCoworker::SetAge(const int InAge)
{
	if (InAge < 0)
	{
		return;
	}

	Age = InAge;
}

int UCoworker::GetAge() const
{
	return Age;
}

void UCoworker::SetName(const std::string& InName)
{
	if (InName.empty())
	{
		return;
	}

	Name = InName;
}

int UCoworker::AddAge(const int InAge)
{
	if (InAge < 0)
	{
		return Age;
	}

	Age += InAge;
	return Age;
}

void UCoworker::SetInfo(const std::string& InName, const int InAge)
{
	SetName(InName);
	SetAge(InAge);
}

void UCoworker::SetHeight(const float InHeight)
{
	if (InHeight < 0.0f)
	{
		return;
	}

	Height = InHeight;
}

void UCoworker::SetWorking(const bool bInWorking)
{
	bWorking = bInWorking;
}
