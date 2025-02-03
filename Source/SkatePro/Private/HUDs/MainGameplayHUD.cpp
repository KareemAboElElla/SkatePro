// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDs/MainGameplayHUD.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/GameplayWidgetBase.h"

void AMainGameplayHUD::BeginPlay()
{
	MainHUDWidget = CreateWidget<UGameplayWidgetBase>(GetOwningPlayerController(), GameplayHUDClass);
	MainHUDWidget->AddToPlayerScreen();
}

void AMainGameplayHUD::UpdateScorePoints(int NewPoints)
{
	if (MainHUDWidget)
	{
		MainHUDWidget->UpdatePointsText(NewPoints);
	}
}
