// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicule.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Math/Vector.h"
#include "MyPlayerController.h"


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
	FVector steering_direction = arrival(targetVector);
	FVector steering_force = truncate(steering_direction,max_force);
	steering_force /= mass;
	FVector acceleration = steering_force;
	FVector newvelocity = truncate(Velocity	+ acceleration, max_speed);	

	Velocity= newvelocity;

	FVector new_forward = Velocity;
	new_forward.Normalize();
	
	SetActorLocation(GetActorLocation() + Velocity);	
	
	SetActorRotation(FRotator(Velocity.Rotation()));

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
// Called to bind functionality to input
void AVehicule::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AVehicule::seek(const FVector & target)
{
	FVector desired_velocity = target - GetActorLocation();
	desired_velocity.Normalize();
	desired_velocity *= max_speed;
	return desired_velocity - Velocity;
}

FVector AVehicule::flee(const FVector & target)
{
	return -seek(target);
}

FVector AVehicule::arrival(const FVector& target)
{
	FVector target_offset = target - GetActorLocation();
	float distance = target_offset.Size();
	float ramped_speed = max_speed * (distance / 400.f);
	if (ramped_speed < 1) {
		ramped_speed = 0;
	}
	GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White, FString::SanitizeFloat(ramped_speed));

	float clipped_speed = FMath::Min(ramped_speed, max_speed);
	FVector desired_velocity = target_offset *(clipped_speed / distance);
	FVector steering = desired_velocity - Velocity;

	return steering;
}

FVector AVehicule::pursue(const AVehicule & target)
{
	return FVector();
}



