// Copyright Epic Games, Inc. All Rights Reserved.

#include "H1Character.h"
#include "H1Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AH1Character

AH1Character::AH1Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f); //충돌 캡슐 크기 설정
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera")); //CreateDefaultSubobject는 언리얼에서 컴포넌트를 초기화하는 표준 방식.  "FirstPersonCamera"는 나중에 블루프린트나 디버그에서 구분 가능한 이름.
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());//SetupAttachment(GetCapsuleComponent())는 이 카메라를 캡슐에 붙인다는 의미야. 그래서 카메라가 플레이어 몸 기준으로 따라다니게 됨.
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // SetRelativeLocation(...)은 카메라의 위치 조절. 약간 앞(-10), 위(60)에 둠.
	FirstPersonCameraComponent->bUsePawnControlRotation = true; //bUsePawnControlRotation = true는 마우스나 패드로 움직일 때 카메라도 회전하도록 설정.

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P")); //Mesh1P는 1인칭 전용으로 보이는 팔(또는 무기)을 담당하는 스켈레탈 메시야.
	Mesh1P->SetOnlyOwnerSee(true); //SetOnlyOwnerSee(true)로 설정해서 본인만 이 메시를 보게 됨. 즉, 멀티플레이어일 경우, 다른 플레이어는 이 팔을 못 봐.
	Mesh1P->SetupAttachment(FirstPersonCameraComponent); //SetupAttachment(FirstPersonCameraComponent)로 팔을 카메라에 붙임 → 카메라 움직일 때 팔도 따라감.
	Mesh1P->bCastDynamicShadow = false; // 아래와 동일
	Mesh1P->CastShadow = false; //CastShadow 및 bCastDynamicShadow를 false로 비활성화하면 그림자가 안 생겨. 이건 성능 최적화 목적이 크고, 1인칭 팔이 지형에 그림자 지면 어색해서 꺼두는 게 일반적이야.
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f)); //SetRelativeLocation은 팔 위치 조정. 카메라 기준으로 약간 아래(-150)로 내려놓은 거지.


	//추가
	PrimaryActorTick.bCanEverTick = true; //이 캐릭터는 매 프레임마다 Tick() 함수를 실행하겠다는 뜻이야
	bIsZooming = false; // 줌상태인가 아닌가
	ZoomFOV = 30.0f; // 줌상태일떄의 FOV 숫자가 작을수록 카메라가 더 좁고 확대댐 이 수치로 줌 구현
	ZoomInterpSpeed = 10.0f; //현재 FOV에서 ZoomFOV까지 부드럽게 전환할 때 사용하는 보간 속도.

}

void AH1Character::BeginPlay() //부모 클래스(ACharacter)의 BeginPlay를 먼저 실행해. 안 해주면 부모 쪽에서 준비해둔 기능들이 빠질 수 있음.
{
	Super::BeginPlay();

	if (FirstPersonCameraComponent) // 카메라 초기화 확인
	{
		DefaultFOV = FirstPersonCameraComponent->FieldOfView; // 현재 카메라의 FOV 값을 DefaultFOV에 저장. 이건 나중에 줌 풀었을때 시야를 어디로 돌아올꺼냐 하는거임
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AH1Character::NotifyControllerChanged() // 컨트롤러가 바뀌었을떄 호출됨 아마도 게임패드나 모바일 키보드 등등 이런거인듯
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AH1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)  // 키연결임 나중에 참고하면될듯
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AH1Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AH1Character::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}//연결안됬으니 뒤지기 싫으면 연결해라 라는 의미

	//추가
	Super::SetupPlayerInputComponent(PlayerInputComponent); // 부모클래스 입력 바인딩 로직 호출

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AH1Character::BeginZoom); //zoom은 우리가 설정에서 마우스 우클릭으로 설정했으니 저거 누르면 BeginZoom 실행하시오 이런뜻 때면 EndZoom
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AH1Character::EndZoom);
}


void AH1Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>(); // 인베디드 인풋 시스템에서 사용하는 입력값 래퍼래 뭔소리임 시부래

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);//이게 전후
		AddMovementInput(GetActorRightVector(), MovementVector.X);//이게 좌우 내부적으로 뭐 있는듯 
	}
}

void AH1Character::Look(const FInputActionValue& Value) // 이건 마우스로 화면 바라보는거
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();//위랑 비슷함 가져오는듯

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X); // Yaw 좌우 회전
		AddControllerPitchInput(LookAxisVector.Y); // Pitch 상하 회전
	}
}
//추가
void AH1Character::Tick(float DeltaTime) // 매 프레임마다 호출함 이게 카메라의 줌 효과를 부드럽게 처리
{
	Super::Tick(DeltaTime);

	if (FirstPersonCameraComponent) // 매 프레임마다 이 포인터 변수에 주소값이 들가있으면 삼단논법에 의해 bIsZooming의 상태에 따라 카메라가 움직임 true가 줌 상태
	{
		float TargetFOV = bIsZooming ? ZoomFOV : DefaultFOV;
		float NewFOV = FMath::FInterpTo( // 이거 유니티에서 카메라 살살살 움직이는거랑 똑같음
			FirstPersonCameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed
		);
		FirstPersonCameraComponent->SetFieldOfView(NewFOV);
	}
}



// 줌 함수 정의
void AH1Character::BeginZoom()
{
	bIsZooming = true;
}

void AH1Character::EndZoom()
{
	bIsZooming = false;
}