// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "DeerCharacter.generated.h"

UCLASS()
class H1_API ADeerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADeerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void StartFleeingFromDirection(const FVector& Direction);

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float DetectionRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float FleeDistance = 2000.f;

	UPROPERTY()
	APawn* PlayerPawn;

	bool bIsFleeing = false;

	FTimerHandle WanderTimerHandle;
	FTimerHandle FleeResetTimerHandle;

	UFUNCTION()
	void Wander();

	UFUNCTION()
	void ResetFleeState();
};
