// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidgetBase.h"
#include "Components/TextBlock.h"

void UGameplayWidgetBase::UpdatePointsText(int NewPoints)
{
	PointsText->SetText(FText::FromString(FString::FromInt(NewPoints)));
}
