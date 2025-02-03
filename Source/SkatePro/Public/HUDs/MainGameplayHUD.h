// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainGameplayHUD.generated.h"

class UGameplayWidgetBase;
/**
 * 
 */
UCLASS()
class SKATEPRO_API AMainGameplayHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayWidgetBase> GameplayHUDClass;

	UPROPERTY(BlueprintReadWrite)
	UGameplayWidgetBase* MainHUDWidget;

	void UpdateScorePoints(int NewPoints);

protected:
	virtual void BeginPlay() override;

	
};
