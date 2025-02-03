// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkateProCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HUDs/MainGameplayHUD.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASkateProCharacter

ASkateProCharacter::ASkateProCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 1200.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	GetCharacterMovement()->GroundFriction = 4.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->MaxAcceleration = 1500.f;
	GetCharacterMovement()->GravityScale = 1.75;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SkateBoard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkateBoard"));
	SkateBoard->SetupAttachment(GetRootComponent());
	CurrentPoints = 0;
}

void ASkateProCharacter::GetUserInputs_Implementation(float& Forward, float& Right)
{
	if (MoveActionBinding)
	{
		const FVector2D InputValue = MoveActionBinding->GetValue().Get<FVector2D>();
		Forward = InputValue.Y;
		Right = InputValue.X;
	}
}

void ASkateProCharacter::GetLegLocations_Implementation(FVector& FrontLeg, FVector& BackLeg)
{
	if (SkateBoard)
	{
		FrontLeg = SkateBoard->GetSocketLocation(FName("FL"));
		BackLeg = SkateBoard->GetSocketLocation(FName("BL"));
	}
}

bool ASkateProCharacter::GetIsJumping_Implementation()
{
	return IsJumping;
}

void ASkateProCharacter::AddPoints_Implementation(int InPoints)
{
	CurrentPoints += InPoints;
	if (AMainGameplayHUD* HUD = Cast<AMainGameplayHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD()))
	{
		HUD->UpdateScorePoints(CurrentPoints);
	}
}

int ASkateProCharacter::GetPoints_Implementation()
{
	return CurrentPoints;
}

void ASkateProCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ASkateProCharacter::Tick(float DeltaTime)
{
	if (SkateBoard)
	{
		AlignSkate();
		Move(0);
		if (IsBoardFlipping)
		{
			FlipSkate();
		}
		UpdateSpeedEffect();
	}
}

void ASkateProCharacter::Jump()
{
	if (!IsJumping)
	{
		if (JumpMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (!AnimInstance->Montage_IsPlaying(JumpMontage))
			{
				AnimInstance->Montage_Play(JumpMontage);

				BlendingOutDelegate.BindUObject(this, &ASkateProCharacter::OnMontageEnded);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, JumpMontage);

				MontageEndedDelegate.BindUObject(this, &ASkateProCharacter::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, JumpMontage);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ASkateProCharacter::OnNotifyReceived);
				//AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UPlayMontageCallbackProxy::OnNotifyEndReceived);
			}
		}
	}
}

void ASkateProCharacter::StopJumping()
{
	Super::StopJumping();
}

void ASkateProCharacter::OnMontageEnded(UAnimMontage* AnimMontage, bool)
{
	IsJumping = false;
}

void ASkateProCharacter::OnNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == FName("StartJumping"))
	{
		Super::Jump();
		IsJumping = true;
		IsBoardFlipping = true;
	}
	else if (NotifyName == FName("EndJumping"))
	{
		IsJumping = false;
		IsBoardFlipping = false;
	}
}

void ASkateProCharacter::AlignSkate()
{

	FVector FrontWheelHit = WheelTrace(FName("FW"));
	FVector BackWheelHit = WheelTrace(FName("BW"));
	FVector FrontWheelRightHit = WheelTrace(FName("FWR"));
	FVector FrontWheelLeftHit = WheelTrace(FName("FWL"));
	
	FRotator BoardRotation;
	BoardRotation.Pitch = UKismetMathLibrary::FindLookAtRotation(BackWheelHit, FrontWheelHit).Pitch;
	BoardRotation.Roll = UKismetMathLibrary::FindLookAtRotation(FrontWheelLeftHit,	FrontWheelRightHit).Pitch;
	
	BoardRotation = FMath::RInterpTo(SkateBoard->GetComponentRotation(), BoardRotation, GetWorld()->DeltaTimeSeconds, 20.f);
	
	SkateBoard->SetRelativeRotation(FRotator(BoardRotation.Pitch, SkateBoard->GetRelativeRotation().Yaw,BoardRotation.Roll));
}

void ASkateProCharacter::FlipSkate()
{
	if (SkateBoard)
	{
		FRotator NewRotation (0);
		NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, 360, GetWorld()->DeltaTimeSeconds, SkateFlipSpeed);
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Green, NewRotation.ToString());
		
		SkateBoard->AddRelativeRotation(NewRotation);
	}
}

void ASkateProCharacter::UpdateSpeedEffect()
{
	float speedRatio = GetVelocity().Length() / GetCharacterMovement()->MaxWalkSpeed;
	FollowCamera->SetFieldOfView(FMath::Lerp(90.f, 120.f, speedRatio));
}

FVector ASkateProCharacter::WheelTrace(FName SocketName)
{
	const FVector SocketLocation = SkateBoard->GetSocketLocation(SocketName);
	FVector SocketLocationStart = SocketLocation;
	SocketLocationStart.Z += 30.f;
	FVector SocketLocationEnd = SocketLocation;
	SocketLocationEnd.Z -= 30.f;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult WheelHit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		WheelHit,
		SocketLocationStart,
		SocketLocationEnd,
		ECC_Visibility,
		TraceParams
	);
	return WheelHit.ImpactPoint;
}


// Input

void ASkateProCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASkateProCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASkateProCharacter::Move);
		MoveActionBinding = &EnhancedInputComponent->BindActionValue(MoveAction);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASkateProCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASkateProCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FVector ForwardDirection = GetActorForwardVector();
		const FVector RightDirection = GetActorRightVector();
		// get right vector 
		const FVector BoardRightDirection = SkateBoard->GetRightVector();
		const FVector BoardFrontDirection = SkateBoard->GetForwardVector();
		
		float Scaler = BoardFrontDirection.Z*3;
		Scaler = FMath::Clamp(Scaler, -0.5f,0.5f);
		
		// add movement 
		MoveForwardVal = FMath::Lerp(MoveForwardVal,MovementVector.Y - Scaler,0.01);
		if (MoveForwardVal<0)
		{
			MoveForwardVal = FMath::Clamp(MoveForwardVal, -100, 0);
		}
		if (MoveForwardVal != 0)
		{
			AddMovementInput(ForwardDirection, MoveForwardVal);
		}
		
		float FinalSlideFactor = SlideFactor *FMath::Clamp(GetVelocity().Length() - 1000, 100, 200);
		AddMovementInput(RightDirection, MovementVector.X * FinalSlideFactor);
		
		FRotator SlideRotator(0);

		SlideRotator.Yaw = MovementVector.X * FinalSlideFactor * 100;
		
		AddActorLocalRotation(SlideRotator);

	}
}

void ASkateProCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}