// SniperOverlayWidget.cpp
#include "USniperOverlayWidget.h"
#include "TimerManager.h"
#include "Animation/WidgetAnimation.h"

void USniperOverlayWidget::PlaySniperTransitionIn()
{
    if (bIsSniperTransitionPlaying) return;
    bIsSniperTransitionPlaying = true;

    if (!FadeInAnim || !ScopeInAnim)
    {
        UE_LOG(LogTemp, Error, TEXT("FadeInAnim or ScopeInAnim is nullptr!"));
        bIsSniperTransitionPlaying = false;
        return;
    }

    PlayAnimation(FadeInAnim);

    float Delay = FadeInAnim->GetEndTime();

    GetWorld()->GetTimerManager().SetTimer(ScopeInTimerHandle, [this]()
        {
            PlayAnimation(ScopeInAnim);
            bIsSniperTransitionPlaying = false;
        }, Delay, false);
}

void USniperOverlayWidget::PlaySniperTransitionOut()
{
    if (bIsSniperTransitionPlaying) return;
    bIsSniperTransitionPlaying = true;

    if (!FadeOutAnim || !ScopeOutAnim)
    {
        UE_LOG(LogTemp, Error, TEXT("FadeOutAnim or ScopeOutAnim is nullptr!"));
        bIsSniperTransitionPlaying = false;
        return;
    }

    PlayAnimation(FadeOutAnim);

    float Delay = FadeOutAnim->GetEndTime();

    GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, [this]()
        {
            PlayAnimation(ScopeOutAnim);
            bIsSniperTransitionPlaying = false;
        }, Delay, false);
}
