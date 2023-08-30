// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SwordsCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class ASoul;
class ATreasure;
class UAnimMontage;
class USwordsOverlay;
class UInputMappingContext;
class UInputAction;
class UGroomComponent;
class AItem;
class UAnimMontage;
class USwordsOverlay;

UCLASS()
class SWORDS_API ASwordsCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASwordsCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

protected:
	
	virtual void BeginPlay() override;

	/** Callbacks for input	*/
	void Move(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value);
	void Walk(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	void Dodge();

	float SprintSpeed = 900.0f;
	float RunSpeed = 600.0f;
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* SwordsContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* WalkAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	

	/** Combat */
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;

	bool CanSheathe();
	bool CanUnSheathe();
	void Unequip();
	void Equip();
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHip();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void EndEquip();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* viewCamera;

private:
	
	bool IsUnoccupied();
	void InitializeSwordsOverlay();
	void SetHUDHealth();

	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* cameraBoom;



	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)	
	UGroomComponent* Eyebrows;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* SheatheMontage;

	ECharacterState CharState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	USwordsOverlay* SwordsOverlay;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
