// Fill out your copyright notice in the Description page of Project Settings.


#include "DeerCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"


// Sets default values
ADeerCharacter::ADeerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Tags.Add(FName("Deer"));
}

// Called when the game starts or when spawned
void ADeerCharacter::BeginPlay()
{
	Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	GetWorldTimerManager().SetTimer(WanderTimerHandle, this, &ADeerCharacter::Wander, 5.0f, true);

}

// Called every frame
void ADeerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!PlayerPawn)
        return;

    if (bIsFleeing || !PlayerPawn) return;

    const float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());

    if (Distance < DetectionRadius)
    {
        const FVector FleeDirection = (GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
        StartFleeingFromDirection(FleeDirection);
    }

}

// Called to bind functionality to input
void ADeerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADeerCharacter::StartFleeingFromDirection(const FVector& Direction)
{
	if (bIsFleeing) return;

	bIsFleeing = true;

	const FVector TargetLocation = GetActorLocation() + Direction * FleeDistance;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(TargetLocation);
	}

	GetWorldTimerManager().ClearTimer(WanderTimerHandle); // 도망 중엔 산책 멈춤
	GetWorldTimerManager().SetTimer(FleeResetTimerHandle, this, &ADeerCharacter::ResetFleeState, 5.0f, false);

}

float ADeerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsFleeing || !DamageCauser) return DamageAmount;

	// 방향 기반 도망 처리
	const FVector HitDirection = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
	StartFleeingFromDirection(HitDirection);

	return DamageAmount;
}

void ADeerCharacter::Wander()
{
	if (bIsFleeing || !PlayerPawn) return;

	float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
	if (Distance < DetectionRadius) return;

	FNavLocation RandomLocation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys && NavSys->GetRandomReachablePointInRadius(GetActorLocation(), 1000.f, RandomLocation))
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->MoveToLocation(RandomLocation.Location);
		}
	}
}

void ADeerCharacter::ResetFleeState()
{
	bIsFleeing = false;

	// 도망 끝났으니 다시 산책 시작
	GetWorldTimerManager().SetTimer(WanderTimerHandle, this, &ADeerCharacter::Wander, 5.0f, true);
}


