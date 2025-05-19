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
	

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ScopeInAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ScopeOutAnim;


	UFUNCTION(BlueprintCallable)
	void PlaySniperTransitionIn();  // 줌 들어올 때
	UFUNCTION(BlueprintCallable)
	void PlaySniperTransitionOut(); // 줌 해제할 때

	FTimerHandle ScopeInTimerHandle;
	FTimerHandle FadeOutTimerHandle;
	bool bIsSniperTransitionPlaying = false;
};
