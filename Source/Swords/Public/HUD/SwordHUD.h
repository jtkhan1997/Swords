// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SwordHUD.generated.h"

/**
 * 
 */
class USwordsOverlay;
UCLASS()
class SWORDS_API ASwordHUD : public AHUD
{
	GENERATED_BODY()
protected:
	//virtual void BeginPlay() override;

	virtual void PreInitializeComponents() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = Sword)
	TSubclassOf<USwordsOverlay> SwordsOverlayClass;

	UPROPERTY()
	USwordsOverlay* SwordsOverlay;

public:
	FORCEINLINE USwordsOverlay* GetSwordsOverlay() const { return SwordsOverlay; }
};
