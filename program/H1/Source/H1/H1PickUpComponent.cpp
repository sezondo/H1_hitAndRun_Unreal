// Copyright Epic Games, Inc. All Rights Reserved.

#include "H1PickUpComponent.h"

UH1PickUpComponent::UH1PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UH1PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UH1PickUpComponent::OnSphereBeginOverlap);
}

void UH1PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AH1Character* Character = Cast<AH1Character>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
