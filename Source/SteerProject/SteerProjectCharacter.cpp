// Copyright Epic Games, Inc. All Rights Reserved.

#include "SteerProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "SteerProjectPlayerController.h"
#include <Kismet/KismetMathLibrary.h>

#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ASteerProjectCharacter::ASteerProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxWalkSpeed = 600;


	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	targetVector = GetActorLocation();
	GetCharacterMovement()->Velocity = FVector(0.f, 0.f, 0.f);
}

void ASteerProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (ASteerProjectPlayerController* PC = Cast<ASteerProjectPlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);

			if (PC->bMoveToMouseCursor)
			{
				targetVector = GetCursorToWorld()->GetComponentLocation();
			}
		}
	}
	FVector steering_force = seek(targetVector);
	steering_force /= 80;
	FVector acceleration = steering_force;
	FVector newvelocity = truncate(GetCharacterMovement()->Velocity + acceleration, GetCharacterMovement()->MaxWalkSpeed);
	GetCharacterMovement()->Velocity = newvelocity;
	FVector new_forward = GetCharacterMovement()->Velocity;
	new_forward.Normalize();
	SetActorLocation(GetActorLocation() + GetCharacterMovement()->Velocity);
	SetActorRotation(FRotator(0, GetCharacterMovement()->Velocity.Rotation().Yaw, GetCharacterMovement()->Velocity.Rotation().Roll));

}


FVector ASteerProjectCharacter::seek(const FVector& target)
{
	FVector target_offset = target - GetActorLocation();
	float distance = target_offset.Size();
	float ramped_speed = (GetCharacterMovement()->MaxWalkSpeed ) * (distance / 500.f);
	if (ramped_speed < 1.5) {
		ramped_speed = 0;
	}

	float clipped_speed = FMath::Min(ramped_speed, GetCharacterMovement()->MaxWalkSpeed );
	FVector desired_velocity = target_offset * (clipped_speed / distance);
	FVector steering = desired_velocity - GetCharacterMovement()->Velocity;

	return steering;
}


FVector ASteerProjectCharacter::GetVelocity() const
{
	return Velocity;
}


FVector ASteerProjectCharacter::truncate(const FVector& vec, const float& m)
{
	if (vec.Size() > m) {
		FVector res = vec;
		res.Normalize();
		res *= m;
		return res;
	}
	return vec;
}