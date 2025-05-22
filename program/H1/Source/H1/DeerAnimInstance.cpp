#include "DeerAnimInstance.h"
#include "DeerCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

void UDeerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* OwningPawn = TryGetPawnOwner();
	if (!OwningPawn) return;

	ADeerCharacter* Deer = Cast<ADeerCharacter>(OwningPawn);
	if (!Deer) return;

	Speed = Deer->GetMovementSpeed();
	bIsFleeing = Deer->IsFleeing();
	bIsWandering = Deer->IsWandering();
	bIsDead = Deer->IsDead();
}
