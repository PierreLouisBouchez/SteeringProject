// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <Components/BoxComponent.h>
#include "Vehicule.generated.h"


UCLASS()
class PROJECTIA_API AVehicule : public APawn
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Vehicule)
	UBoxComponent *Boxcomponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Vehicule)
	float max_force;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Vehicule)
	float max_speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Vehicule)
	float mass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Vehicule)
	FVector targetVector;

	FVector Velocity;

	AVehicule();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector seek(const FVector & target);
	FVector flee(const FVector & target);
	FVector arrival(const FVector& target);
	FVector pursue(const AVehicule & target);
	FVector truncate(const FVector& vec, const float& m);
};
