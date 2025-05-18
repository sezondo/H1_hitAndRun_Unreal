// SniperOverlayWidget.cpp
#include "USniperOverlayWidget.h"

void USniperOverlayWidget::PlayFadeIn()
{
	if (FadeInAnim)
	{
		PlayAnimation(FadeInAnim);
	}
}

void USniperOverlayWidget::PlayFadeOut()
{
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);
	}
}
