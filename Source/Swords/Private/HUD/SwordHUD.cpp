// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SwordHUD.h"
#include "HUD/SwordsOverlay.h"

void ASwordHUD::PreInitializeComponents()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && SwordsOverlayClass)
		{
			SwordsOverlay = CreateWidget<USwordsOverlay>(Controller, SwordsOverlayClass);
			SwordsOverlay->AddToViewport();
		}
	}
}
