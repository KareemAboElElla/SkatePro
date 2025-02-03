// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ObstacleActorBase.h"
#include "Components/BoxComponent.h"
#include "Interfaces/PlayerInterface.h"

// Sets default values
AObstacleActorBase::AObstacleActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ObstacleCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ObstacleCollision"));
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(GetRootComponent());
	ObstacleCollision->SetupAttachment(ObstacleMesh);
	ObstaclePoints = 1;

}

// Called when the game starts or when spawned
void AObstacleActorBase::BeginPlay()
{
	Super::BeginPlay();
	ObstacleCollision->OnComponentBeginOverlap.AddDynamic(this,&AObstacleActorBase::BeginOverlap);
	ObstacleCollision->OnComponentEndOverlap.AddDynamic(this, &AObstacleActorBase::EndOverlap);
	
}

void AObstacleActorBase::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AObstacleActorBase::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetIsJumping(OtherActor))
		{
			IPlayerInterface::Execute_AddPoints(OtherActor,ObstaclePoints);
		}
	}
}


