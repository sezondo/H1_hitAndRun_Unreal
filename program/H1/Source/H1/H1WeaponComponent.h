// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "H1WeaponComponent.generated.h"



class AH1Character;
class UNiagaraSystem;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class H1_API UH1WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	/*
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AH1Projectile> ProjectileClass;
	*/
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Sets default values for this component's properties */
	UH1WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AH1Character* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	AH1Character* Character;

	//이거 총알 쿨타임
	float LastFireTime; // 마지막 발사 시간
	float FireCooldown = 1.9f; // 쿨타임 (초)

	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleFlashEffect;

	

	// 탄도 관련
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BulletSpeed = 20000.f; // 속도

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BulletDamage = 80.f; // 데미지

	// 탄도 라인트레이스 함수 선언
	void BallisticLineTrace();

	// 머즐 플래시 함수 선언
	void PlayMuzzleFlash();

	
	

	// Niagara 기반 피격 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* DefaultImpactNiagaraFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* BloodImpactNiagaraFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UCameraShakeBase> FireCameraShake;



};
