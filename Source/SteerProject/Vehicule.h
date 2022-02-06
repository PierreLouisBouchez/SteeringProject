// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/BoxComponent.h>
#include "Actions.h"
#include "Vehicule.generated.h"

UCLASS()
class STEERPROJECT_API AVehicule : public ACharacter
{
	GENERATED_BODY()

private:
	FVector Velocity;


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Steering)
	UBoxComponent* Boxcomponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Steering)
	float max_force;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Steering)
	float max_speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Steering)
	float mass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Steering)
	TEnumAsByte<EActions> action ;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Steering)
	ACharacter * targetcharacter;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Steering)
	AActor * targetactor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Steering)
	TArray<AActor*> targetList;

	int circuitindex = 0;
	int sens = 1;

	AVehicule();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector seek(const FVector& target);
	FVector flee(const FVector& target);
	FVector arrival(const FVector& target);
	FVector actionchoose();


	void circuit();

	void oneway();

	void twoways();


	FVector pursue(const ACharacter& target);
	FVector evade(const ACharacter& target);


	FVector truncate(const FVector& vec, const float& m);
	virtual FVector GetVelocity() const override;;

};