// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleActorBase.generated.h"

class UBoxComponent;

UCLASS()
class SKATEPRO_API AObstacleActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObstacleActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UStaticMeshComponent* ObstacleMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* ObstacleCollision;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int ObstaclePoints;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex);

};
