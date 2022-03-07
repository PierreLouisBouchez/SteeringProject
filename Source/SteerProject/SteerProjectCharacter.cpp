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
	GetCharacterMovement()->MaxWalkSpeed = 13;


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
				PC->bMoveToMouseCursor = false;
				targetVector = GetCursorToWorld()->GetComponentLocation();

				PathFinding(targetVector);
				targetList=oneway();
				targetVector = targetList[0];
				circuitindex = 0;
			}
		}
	}

	

	
	if (targetList.Num() == 0) {
		return;
	}
	FVector steering_force = seek(targetVector);
	steering_force = truncate(steering_force, 500);
	steering_force /= 10;
	FVector acceleration = steering_force;
	FVector newvelocity = truncate(GetCharacterMovement()->Velocity + acceleration, GetCharacterMovement()->MaxWalkSpeed);
	GetCharacterMovement()->Velocity = newvelocity;
	FVector new_forward = GetCharacterMovement()->Velocity;
	new_forward.Normalize();
	SetActorLocation(GetActorLocation() + GetCharacterMovement()->Velocity);
	SetActorRotation(FRotator(0, GetCharacterMovement()->Velocity.Rotation().Yaw, GetCharacterMovement()->Velocity.Rotation().Roll));
	CurrentVelocity = GetCharacterMovement()->Velocity;
	
	
	FVector target_offset = targetVector - GetActorLocation();
	float distance = target_offset.Size();
	if (distance < 150.f) {
		if (circuitindex == targetList.Num()) {
			return;
		}
		targetVector = targetList[circuitindex];
		circuitindex++;
	}
}




FVector ASteerProjectCharacter::seek(const FVector& target)
{
	FVector target_offset = target - GetActorLocation();
	float distance = target_offset.Size();
	float ramped_speed = (GetCharacterMovement()->MaxWalkSpeed ) * (distance / 100.f);
	if (ramped_speed < 1) {
		ramped_speed = 0;
	}

	float clipped_speed = FMath::Min(ramped_speed, GetCharacterMovement()->MaxWalkSpeed );
	FVector desired_velocity = target_offset * (clipped_speed / distance);
	desired_velocity.Z = 0;
	FVector steering = desired_velocity - GetCharacterMovement()->Velocity;

	return steering;
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

void ASteerProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	Map= Scenebuilder->GetMap();
	targetVector = GetActorLocation();
	GetCharacterMovement()->Velocity = FVector(0.f, 0.f, 0.f);
}

void ASteerProjectCharacter::PathFinding(FVector Target)
{		
	int xtarget = FloorHundred(Target.X);
	int ytarget = FloorHundred(Target.Y);

	if (xtarget < 0 || xtarget >31 || ytarget < 0 || ytarget >31) {
		return ;
	}
	Node& target = Map[xtarget][ytarget];
	Node& start = Map[FloorHundred(GetActorLocation().X)][ FloorHundred(GetActorLocation().Y)];
	int neightbour[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};

	Node& currentNode = start;
	TArray<Node> Open;
	Open.Add(start);
	int min = 0;

	while (Open.Num()>0)
	{
		min = 0;		
		for (int w = 0; w < Open.Num() - 1; w++) {
			if (Open[w].f() < Open[min].f()) {
				min = w;
			}
		}
		currentNode = Open[min];
		currentNode.IsTraited=true;		
		Open.RemoveAt(min);		

		if (currentNode.x == target.x && currentNode.y == target.y) {
			return;
		}

		for (size_t j = 0; j < 4; j++)
		{			
			int x = currentNode.x + neightbour[j][0];
			int y = currentNode.y + neightbour[j][1];			
			Node& currneigh = Map[x][y];
			if (currneigh.pass && !currneigh.IsTraited && !currneigh.IsWaiting) {
				currneigh.Parentx = currentNode.x;
				currneigh.Parenty = currentNode.y;
				currneigh.h = Manhattan(currneigh, target);
				currneigh.g = currentNode.g+1;
				currneigh.IsWaiting = true;
				Open.Add(currneigh);
			}
		}	
	}
}



int ASteerProjectCharacter::Manhattan(Node start, Node End) {
	return abs(End.x - start.x) + abs(End.y - start.y);
}




int ASteerProjectCharacter::FloorHundred(float a) {
	int res = 0;
	if (a < 0) {
		res = (a / 100) - 0.5;
	}
	else if (a > 0) {
		res = (a / 100) + 0.5;
	}
	return res;

}

TArray<FVector> ASteerProjectCharacter::oneway()
{
	TArray<FVector> List;
	Node curr= Map[FloorHundred(targetVector.X)][FloorHundred(targetVector.Y)];
	int g = curr.g;
	while(g>1 ) {
		List.Insert(FVector(curr.x * 100, curr.y * 100, GetActorLocation().Z),0);
		curr = Map[curr.Parentx][curr.Parenty];
		g--;
	}
	Map = Scenebuilder->GetMap();

	return List;
}
