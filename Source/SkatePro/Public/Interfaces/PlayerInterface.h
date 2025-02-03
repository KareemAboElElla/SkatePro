// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKATEPRO_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void GetUserInputs(float& Forward, float& Right);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetLegLocations(FVector& FrontLeg, FVector& BackLeg);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsJumping();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddPoints(int InPoints);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int GetPoints();
};
