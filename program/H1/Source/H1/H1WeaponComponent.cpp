// Copyright Epic Games, Inc. All Rights Reserved.


#include "H1WeaponComponent.h"
#include "H1Character.h"
#include "H1Projectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UH1WeaponComponent::UH1WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

}


void UH1WeaponComponent::Fire()
{
	if (!Character || !Character->GetController()) return;
	if (!Character->IsZooming()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (CurrentTime - LastFireTime < FireCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("총알 쿨타임 중... %.2f초 남음"), FireCooldown - (CurrentTime - LastFireTime));
		return;
	}

	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass is NULL"));
		return;
	}

	// 발사 회전: 카메라 기준
	const FRotator SpawnRotation = Character->GetControlRotation();

	// 발사 위치: 무기 메시의 Muzzle 소켓
	USkeletalMeshComponent* Mesh = Character->GetMesh1P(); // CharacterMesh0 기준
	if (!Mesh->DoesSocketExist("Muzzle"))
	{
		UE_LOG(LogTemp, Error, TEXT("Muzzle socket not found on mesh"));
		return;
	}

	const FVector SpawnLocation = Mesh->GetSocketLocation("Muzzle");

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	AActor* Projectile = World->SpawnActor<AH1Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (Projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile Spawn Success: %s"), *Projectile->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Projectile Spawn Failed"));
	}

	LastFireTime = World->GetTimeSeconds();

	// 사운드
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	// 애니메이션
	if (FireAnimation)
	{
		if (UAnimInstance* Anim = Character->GetMesh1P()->GetAnimInstance())
		{
			Anim->Montage_Play(FireAnimation, 1.f);
		}
	}
}


bool UH1WeaponComponent::AttachWeapon(AH1Character* TargetCharacter)
{
	Character = TargetCharacter;
	/*
	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UH1WeaponComponent>())
	{
		return false;
	}
	*/

	if (!Character)
	{
		return false;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UH1WeaponComponent::Fire);
		}
	}
	/*
	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UH1WeaponComponent::Fire);
		}
	}
	*/
	return true;
}

void UH1WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ensure we have a character owner
	if (Character != nullptr)
	{
		// remove the input mapping context from the Player Controller
		if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(FireMappingContext);
			}
		}
	}

	// maintain the EndPlay call chain
	Super::EndPlay(EndPlayReason);
}