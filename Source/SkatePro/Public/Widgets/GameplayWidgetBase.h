// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class SKATEPRO_API UGameplayWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UTextBlock* PointsText;

	void UpdatePointsText(int NewPoints);
};
