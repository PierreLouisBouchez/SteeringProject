// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicule.h"
#include "GameFramework/Character.h"

#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Math/Vector.h"



// Sets default values
AVehicule::AVehicule()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Velocity = FVector(200.f, -100.f, 0.f);
	Boxcomponent = CreateDefaultSubobject<UBoxComponent>(TEXT("vehicule"));
	Boxcomponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AVehicule::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVehicule::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (targetcharacter == nullptr) {
		return;
	}
	FVector steering_direction = actionchoose();
	FVector steering_force = truncate(steering_direction, max_force);
	steering_force /= mass;
	FVector acceleration = steering_force;
	FVector newvelocity = truncate(Velocity + acceleration, max_speed);
	Velocity = newvelocity;
	FVector new_forward = Velocity;
	new_forward.Normalize();
	SetActorLocation(GetActorLocation() + Velocity);
	SetActorRotation(FRotator(Velocity.Rotation()));

}

FVector AVehicule::actionchoose()
{
	switch (action)
	{
	case Flee:
		return flee(targetcharacter->GetActorLocation());
		break;

	case Seek:
		return seek(targetcharacter->GetActorLocation());
		break;

	case Pursuit:
		return pursue(*targetcharacter);
		break;

	case Arrival:
		return flee(targetcharacter->GetActorLocation());
		break;
	case Evade:
		return evade(*targetcharacter);
		break;
	case Circuit:
		circuit();
		return seek(targetactor->GetActorLocation());
		break;
	case OneWay:
		oneway();
		return arrival(targetactor->GetActorLocation());	
		break;
	default:
		break;
	}
	return FVector();
}

void AVehicule::circuit()
{
	FVector target_offset = targetactor->GetActorLocation() - GetActorLocation();
	float distance = target_offset.Size();
	GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White, FString::Printf(TEXT("%f"),distance));

	if (distance < 200.f) {
		if (circuitindex == targetList.Num()-1) {
			circuitindex = 0;
		}
		targetactor = targetList[circuitindex];		
		circuitindex++;
	}
}


void AVehicule::oneway()
{
	FVector target_offset = targetactor->GetActorLocation() - GetActorLocation();
	float distance = target_offset.Size();

	if (distance < 200.f) {
		if (circuitindex == targetList.Num() - 1) {
			return;
		}
		targetactor = targetList[circuitindex];
		circuitindex++;
	}
}



FVector AVehicule::truncate(const FVector& vec, const float& m)
{
	if (vec.Size() > m) {
		FVector res = vec;
		res.Normalize();
		res *= m;
		return res;
	}
	return vec;
}

FVector AVehicule::GetVelocity() const
{
	return Velocity;
}

// Called to bind functionality to input
void AVehicule::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AVehicule::seek(const FVector& target)
{
	FVector desired_velocity = target - GetActorLocation();
	desired_velocity.Normalize();
	desired_velocity *= max_speed;
	return desired_velocity - Velocity;
}

FVector AVehicule::flee(const FVector& target)
{
	return -seek(target);
}

FVector AVehicule::arrival(const FVector& target)
{
	FVector target_offset = target - GetActorLocation();
	float distance = target_offset.Size();
	float ramped_speed = max_speed * (distance / 500.f);
	if (ramped_speed < 1) {
		ramped_speed = 0;
	}

	float clipped_speed = FMath::Min(ramped_speed, max_speed);
	FVector desired_velocity = target_offset * (clipped_speed / distance);
	FVector steering = desired_velocity - Velocity;

	return steering;
}

FVector AVehicule::pursue(const ACharacter& target)
{


	float angle = FVector::DotProduct(target.GetVelocity().GetSafeNormal(), (GetActorLocation()-target.GetActorLocation()).GetSafeNormal());
	float c = 1-angle;
	GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White, FString::Printf(TEXT("%f"), c));

	return seek(target.GetActorLocation() + target.GetVelocity() * 1.f);
}

FVector AVehicule::evade(const ACharacter& target)
{
	FVector targetPredictedPosition = target.GetActorLocation() + target.GetVelocity() * 1.f;

	FVector desiredVelocity = (GetActorLocation() - targetPredictedPosition);
	desiredVelocity.Normalize();
	desiredVelocity*= max_speed;
	return desiredVelocity;
}

