// Fill out your copyright notice in the Description page of Project Settings.


#include "DeerCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"



// Sets default values
ADeerCharacter::ADeerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Tags.Add(FName("Deer"));

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ADeerCharacter::OnPlayerOverlap);
}


// Called when the game starts or when spawned
void ADeerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	GetWorldTimerManager().SetTimer(WanderTimerHandle, this, &ADeerCharacter::Wander, 5.0f, true);

}

// Called every frame
void ADeerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerPawn || bIsFleeing || bIsDead) return;

	const float CurrentSpeed = GetVelocity().Size();
	const bool bIsMoving = CurrentSpeed > 10.0f;

	if (!bIsFleeing && !bIsDead)
	{
		bIsWandering = bIsMoving;
	}

	if (!bIsFleeing)
	{
		const float Distance = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
		if (Distance < DetectionRadius)
		{
			const FVector FleeDirection = (GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
			StartFleeingFromDirection(FleeDirection);
		}
	}

}

// Called to bind functionality to input
void ADeerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADeerCharacter::StartFleeingFromDirection(const FVector& Direction)
{
	if (bIsFleeing || bIsDead) return;

	bIsFleeing = true;

	const FVector TargetLocation = GetActorLocation() + Direction * FleeDistance;


	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.f;

		AIController->MoveToLocation(TargetLocation);
	}

	GetWorldTimerManager().ClearTimer(WanderTimerHandle); // 도망 중엔 산책 멈춤
	GetWorldTimerManager().SetTimer(FleeResetTimerHandle, this, &ADeerCharacter::ResetFleeState, 5.0f, false);

}

float ADeerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.f;

	CurrentHealth -= DamageAmount;

	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;
		GetController()->StopMovement();
		GetWorldTimerManager().ClearAllTimersForObject(this);

		if (DeathMontage && GetMesh())
		{
			if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
			{
				AnimInst->Montage_Play(DeathMontage);
			}
		}

		return DamageAmount;
	}

	if (!bIsFleeing && DamageCauser)
	{
		const FVector HitDirection = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
		StartFleeingFromDirection(HitDirection);
	}

	return DamageAmount;
}

void ADeerCharacter::Wander()
{
	if (bIsFleeing || bIsDead || !PlayerPawn) return;

	float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
	if (Distance < DetectionRadius) return;

	FNavLocation RandomLocation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys && NavSys->GetRandomReachablePointInRadius(GetActorLocation(), 2000.f, RandomLocation))
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			bIsWandering = true;

			GetCharacterMovement()->MaxWalkSpeed = 100.f;

			AIController->MoveToLocation(RandomLocation.Location);
		}
	}
}

void ADeerCharacter::ResetFleeState()
{
	bIsFleeing = false;
	bIsWandering = false;

	if (!bIsDead)
	{
		GetWorldTimerManager().SetTimer(WanderTimerHandle, this, &ADeerCharacter::Wander, 5.0f, true);
	}
}

float ADeerCharacter::GetMovementSpeed() const
{
	return GetVelocity().Size();
}

void ADeerCharacter::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsDead && OtherActor && OtherActor->ActorHasTag(FName("Player")))
	{
		Destroy();
	}
}


