// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DeerAnimInstance.generated.h"

class ADeerCharacter;

UCLASS()
class H1_API UDeerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Deer")
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Deer")
	bool bIsFleeing;

	UPROPERTY(BlueprintReadOnly, Category = "Deer")
	bool bIsWandering;

	UPROPERTY(BlueprintReadOnly, Category = "Deer")
	bool bIsDead;

};
