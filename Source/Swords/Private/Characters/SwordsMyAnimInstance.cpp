// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SwordsMyAnimInstance.h"
#include "Characters/SwordsCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USwordsMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SwordsCharacter = Cast<ASwordsCharacter>(TryGetPawnOwner());
	if (SwordsCharacter) {
		SwordsCharacterMovement = SwordsCharacter->GetCharacterMovement();
	}
}

void USwordsMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SwordsCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(SwordsCharacterMovement->Velocity);
		isFalling = SwordsCharacterMovement->IsFalling();
		CharState = SwordsCharacter->GetCharacterState();
		ActionState = SwordsCharacter->GetActionState();
		DeathPose = SwordsCharacter->GetDeathPose();
	}
}
