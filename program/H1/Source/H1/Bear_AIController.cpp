// Fill out your copyright notice in the Description page of Project Settings.


#include "Bear_AIController.h"
#include "BearCharacter.h"
#include <AIController.h>
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


const FName ABear_AIController::HomePosKey(TEXT("HomePos"));
const FName ABear_AIController::PatrolPosKey(TEXT("PatrolPos"));

ABear_AIController::ABear_AIController()
{
    static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObj(TEXT("/Script/AIModule.BlackboardData'/Game/bear-animated-urso-3d-model-free/Blueprints/BB_BearCharacter.BB_BearCharacter'"));
    if (BBObj.Succeeded())
    {
        //UE_LOG(LogTemp, Warning, TEXT("bbobj 연결 성공: %s"), *BBObj.Object->GetName());
        BBAsset = BBObj.Object;
    }
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObj(TEXT("/Script/AIModule.BehaviorTree'/Game/bear-animated-urso-3d-model-free/Blueprints/BT_BearCharacter.BT_BearCharacter'"));
    if (BTObj.Succeeded())
    {
        BTAsset = BTObj.Object;
    }


	RepeatInterval = 3.0f;
}

void ABear_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("곰 AIController가 Possess 성공: %s"), *InPawn->GetName());

    UBlackboardComponent* bbComponent = Blackboard;
    if(UseBlackboard(BBAsset, bbComponent))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Behavior Tree 시작됨"));
		bbComponent->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
        if(!RunBehaviorTree(BTAsset))
        {
            UE_LOG(LogTemp, Error, TEXT("Alcontroller couldn't run behavior tree"));
        }
    }

	//GetWorld()->GetTimerManager().SetTimer(RepeatTimeHandle, this, &ABear_AIController::OnRepeatTimer, RepeatInterval, true);
}

void ABear_AIController::OnUnPossess()
{
	Super::OnUnPossess();
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimeHandle);
}

void ABear_AIController::BeginPlay()
{
	Super::BeginPlay();
	// 소유객체 가져오기
    me = Cast<ABearCharacter>(GetOwner());
    // AAIController 할당하기
	//ai = Cast<AAIController>(me->GetController());
}
/*
void ABear_AIController::OnRepeatTimer()
{
	auto CurrentPawn = GetPawn();
	ABCHECK(nullptr != CurrentPawn);

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (nullptr == NavSystem) return;
	FNavLocation nextLocation;
	if (NavSystem->GetRandomPointInNavigableRadius(CurrentPawn->GetActorLocation(), 2000.0f, nextLocation))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, nextLocation.Location);
		//ai->MoveToLocation(nextLocation.Location);
	}
}
*/
/*
void ABear_AIController::OnRepeatTimer()
{
    APawn* CurrentPawn = GetPawn();
    if (!CurrentPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("CurrentPawn is null!"));
        return;
    }

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
    if (!NavSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("NavSystem is null!"));
        return;
    }

    FNavLocation NextLocation;

    if (NavSystem->GetRandomPointInNavigableRadius(CurrentPawn->GetActorLocation(), 2000.0f, NextLocation))
    {
        UE_LOG(LogTemp, Warning, TEXT("곰 이동 위치: %s"), *NextLocation.Location.ToString());

        // 더 정확한 이동 처리
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalLocation(NextLocation.Location);
        MoveRequest.SetAcceptanceRadius(5.0f);

        FNavPathSharedPtr NavPath;
        EPathFollowingRequestResult::Type Result = MoveTo(MoveRequest, &NavPath);

        UE_LOG(LogTemp, Warning, TEXT("곰 MoveTo 요청 결과: %d"), static_cast<int32>(Result));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("랜덤 위치 탐색 실패"));
    }
}*/

void ABear_AIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    ABearCharacter* Bear = Cast<ABearCharacter>(ControlledPawn);
    if (!Bear) return;

    // Blackboard에서 gunshot 상태 가져오기
    bool bGunshotHeard = Blackboard->GetValueAsBool("bHeardGunshot");

    Bear->bHeardGunshot = bGunshotHeard;

    float DesiredSpeed = bGunshotHeard ? 500.f : 200.f;

    if (Bear->GetCharacterMovement()->MaxWalkSpeed != DesiredSpeed)
    {
        Bear->GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed;
    }
}
