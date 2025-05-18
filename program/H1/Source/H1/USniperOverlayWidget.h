// SniperOverlayWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "USniperOverlayWidget.generated.h"

UCLASS()
class H1_API USniperOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void PlayFadeIn();

	UFUNCTION(BlueprintCallable)
	void PlayFadeOut();

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;
};
