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

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float DetectionRadius = 3000.f; //플레이어 감지 범위

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float FleeDistance = 5000.f; //도망거리

	UPROPERTY()
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY()
	APawn* PlayerPawn;

	bool bIsFleeing = false;
	bool bIsDead = false;

	FTimerHandle WanderTimerHandle;
	FTimerHandle FleeResetTimerHandle;

	UFUNCTION()
	void Wander();

	UFUNCTION()
	void ResetFleeState();

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.f;


public:
	// 애니메이션 블루프린트에서 읽을 상태 변수들
	bool IsDead() const { return bIsDead; }
	bool IsFleeing() const { return bIsFleeing; }
	bool IsWandering() const { return bIsWandering; }
	float GetMovementSpeed() const;

private:
	bool bIsWandering = false; // 산책 중인지 여부




};
