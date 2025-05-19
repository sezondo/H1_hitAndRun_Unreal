// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "H1WeaponComponent.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"
#include "Logging/LogMacros.h"

#include "H1Character.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AH1Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	UH1WeaponComponent* WeaponComponent;

	UPROPERTY()
	UUserWidget* SniperOverlay;

	// .h
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* FootstepSound;

	float StepInterval = 0.4f;
	float TimeSinceLastStep = 0.0f;
	bool bFootstepPlaying = false;

	UPROPERTY()
	UAudioComponent* FootstepAudioComp;


	

	
	// 줌 관련 변수들 선언
private:
	bool bIsZooming;
	float DefaultFOV;
	float ZoomFOV;
	float ZoomInterpSpeed;

	

	// 카메라 참조 (있으면 생략, 없으면 추가)
/*protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FirstPersonCameraComponent;
	*/
	// 함수 선언
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void BeginZoom();
	void EndZoom();

public:
	bool IsZooming() const;


public:
	AH1Character();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void NotifyControllerChanged() override;
	//virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override; // 지움 위에꺼 더 많이쓴데
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SniperOverlayClass;

	//UUserWidget* SniperOverlay;

	void StartZoom();
	void StopZoom();

};

