// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Actions.generated.h"


UENUM(BlueprintType)
enum EActions
{
    Seek   UMETA(DisplayName = "Seek"),
    Flee  UMETA(DisplayName = "Flee"),
    Pursuit  UMETA(DisplayName = "Pursuit"),
    Evade UMETA(DisplayName = "Evade"),
    Arrival UMETA(DisplayName = "Arrival"),
    Circuit UMETA(DisplayName = "Circuit"),
    OneWay UMETA(DisplayName = "OneWay"),
    TwoWays UMETA(DisplayName = "TwoWays")

};