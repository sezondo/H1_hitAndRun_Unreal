// Copyright Epic Games, Inc. All Rights Reserved.


#include "H1WeaponComponent.h"
#include "H1Character.h"

#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DeerCharacter.h"

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

	LastFireTime = CurrentTime;

	// 1. 머즐 플래시 이펙트
	PlayMuzzleFlash();

	// 2. 탄도 라인트레이스
	BallisticLineTrace();

	//카메라 반동
	if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
	{
		if (!FireCameraShake)
		{
			UE_LOG(LogTemp, Error, TEXT("FireCameraShake is NULL!"));
		}
		else if (!PC->PlayerCameraManager)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerCameraManager is NULL!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Playing Camera Shake: %s"), *FireCameraShake->GetName());
			PC->PlayerCameraManager->StartCameraShake(FireCameraShake);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is NULL!"));
	}


	// 발사 위치: 무기 메시의 Muzzle 소켓
	USkeletalMeshComponent* Mesh = Character->GetMesh1P(); // CharacterMesh0 기준
	if (!Mesh->DoesSocketExist("Muzzle"))
	{
		UE_LOG(LogTemp, Error, TEXT("Muzzle socket not found on mesh"));
		return;
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

	// Deer 감지 로직
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeerCharacter::StaticClass(), OutActors);

	for (AActor* Actor : OutActors)
	{
		ADeerCharacter* Deer = Cast<ADeerCharacter>(Actor);
		if (Deer && FVector::Dist(Deer->GetActorLocation(), Character->GetActorLocation()) < 10000.f) // 총소리 감지
		{
			const FVector FleeDirection = (Deer->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
			Deer->StartFleeingFromDirection(FleeDirection);
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


void UH1WeaponComponent::PlayMuzzleFlash()
{
	if (!MuzzleFlashEffect || !Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh1P();
	if (!Mesh || !Mesh->DoesSocketExist("Muzzle")) return;

	FVector MuzzleLoc = Mesh->GetSocketLocation("Muzzle");
	FRotator MuzzleRot = Mesh->GetSocketRotation("Muzzle");

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		MuzzleFlashEffect,
		MuzzleLoc,
		MuzzleRot
	);
}

//이거 곰 사슴 추가되면 추가해해ㅐㅐㅐㅐ

void UH1WeaponComponent::BallisticLineTrace()
{
	USkeletalMeshComponent* Mesh = Character->GetMesh1P();
	if (!Mesh || !Mesh->DoesSocketExist("Muzzle")) return;

	FVector Start = Mesh->GetSocketLocation("Muzzle");
	FVector Velocity = Character->GetFirstPersonCameraComponent()->GetForwardVector() * BulletSpeed;
	FVector Gravity = FVector(0, 0, -980.f);

	float DeltaTime = 0.02f;
	float MaxTime = 2.f;
	FVector Current = Start;

	for (float Time = 0.f; Time < MaxTime; Time += DeltaTime)
	{
		FVector Next = Current + Velocity * DeltaTime + 0.5f * Gravity * DeltaTime * DeltaTime;
		Velocity += Gravity * DeltaTime;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Current, Next, ECC_Visibility))
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("총알 충돌: %s"), *HitActor->GetName());

				// 데미지 적용
				UGameplayStatics::ApplyPointDamage(
					HitActor,
					BulletDamage,
					Velocity.GetSafeNormal(),
					Hit,
					Character->GetController(),
					Character,
					nullptr
				);
			}

			// 피격 이펙트 선택
			UNiagaraSystem* ImpactFX = DefaultImpactNiagaraFX;

			if (HitActor && (HitActor->ActorHasTag("Deer") || HitActor->ActorHasTag("Bear")))
			{
				ImpactFX = BloodImpactNiagaraFX;
			}

			if (ImpactFX)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					ImpactFX,
					Hit.ImpactPoint,
					Hit.ImpactNormal.Rotation()
				);
			}

			break; // 첫 충돌만 처리
		}

		Current = Next;
	}
}
