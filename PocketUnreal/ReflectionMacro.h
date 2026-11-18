#pragma once

#include "UClass.h"

#define GENERATED_BODY(Class, SuperClass) \
public: \
	using ThisClass = Class; \
	using Super = SuperClass; \
	static const UClass* StaticClass()

#define UPROPERTY()

#define UFUNCTION()
