// Fill out your copyright notice in the Description page of Project Settings.


#include "Bear_BTTaskNode.h"
#include "Bear_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "BearCharacter.h"


UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
    NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(ControllingPawn == nullptr) return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
    if(NavSystem == nullptr) return EBTNodeResult::Failed;

    FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ABear_AIController::HomePosKey);
    FNavLocation NextPatrol;

    if(NavSystem->GetRandomPointInNavigableRadius(Origin, 2000.0f, NextPatrol))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(ABear_AIController::PatrolPosKey, NextPatrol.Location);
        return EBTNodeResult::Succeeded;
    }

  
    

    return EBTNodeResult::Failed;
}