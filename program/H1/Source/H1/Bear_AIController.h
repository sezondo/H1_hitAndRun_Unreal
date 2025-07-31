// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Bear_AIController.generated.h"

/**
 * 
 */
UCLASS()
class H1_API ABear_AIController : public AAIController
{
	GENERATED_BODY()
	

public:
	ABear_AIController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	class ABear_AIController* ai;
	UPROPERTY()
	class ABearCharacter* me;

private:
	void OnRepeatTimer();

	FTimerHandle RepeatTimeHandle;
	float RepeatInterval;

	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
};