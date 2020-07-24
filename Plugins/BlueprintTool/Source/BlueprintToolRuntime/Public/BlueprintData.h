// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintData.generated.h"

/**
 * 
 */
UCLASS()
class BLUEPRINTTOOLRUNTIME_API UBlueprintData : public UObject
{
	GENERATED_BODY()
	
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
		class UEdGraph* EdGraph;
#endif // WITH_EDITORONLY_DATA
};
