// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SwordsCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Animation/AnimMontage.h"
#include "HUD/SwordHUD.h"
#include "HUD/SwordsOverlay.h"

// Sets default values
ASwordsCharacter::ASwordsCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(GetRootComponent());
	cameraBoom->TargetArmLength = 300.0f;

	viewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	viewCamera->SetupAttachment(cameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair ->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("Head");
	
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("Head");

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASwordsCharacter::Tick(float DeltaTime)
{
	if (Attributes && SwordsOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SwordsOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASwordsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::Move);
		EnhancedInputComp->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::Sprint);
		EnhancedInputComp->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::Walk);
		EnhancedInputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::Look);
		EnhancedInputComp->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::Jump);
		EnhancedInputComp->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::EKeyPressed);
		EnhancedInputComp->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::Attack);
		EnhancedInputComp->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASwordsCharacter::Dodge);
	}

}

float ASwordsCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASwordsCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASwordsCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASwordsCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SwordsOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SwordsOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASwordsCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SwordsOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SwordsOverlay->SetGold(Attributes->GetGold());
	}
}

void ASwordsCharacter::BeginPlay()
{
	Super::BeginPlay();
	PreInitializeComponents();

	Tags.Add(FName("SwordsCharacter"));
	Tags.Add(FName("EngageableTarget"));

	InitializeSwordsOverlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(SwordsContext, 0);
		}
	}
}

void ASwordsCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

void ASwordsCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotate(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotate).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotate).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASwordsCharacter::Sprint(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (const bool MovementVector = Value.Get<bool>()) {
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void ASwordsCharacter::Walk(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (const bool MovementVector = Value.Get<bool>()) {
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void ASwordsCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void ASwordsCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon) {
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else {
		if (CanSheathe()) {
			Unequip();
		}
		else if (CanUnSheathe()) {
			Equip();
		}
	}
}

void ASwordsCharacter::Attack()
{
	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attack;
	}
}

void ASwordsCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && SwordsOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SwordsOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASwordsCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("hand_r_socket"), this, this);
	CharState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void ASwordsCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASwordsCharacter::CanAttack() {
	return(ActionState == EActionState::EAS_Unoccupied && CharState != ECharacterState::ECS_Unequipped);
}

bool ASwordsCharacter::CanSheathe()
{
	return(ActionState == EActionState::EAS_Unoccupied && CharState != ECharacterState::ECS_Unequipped);
}

bool ASwordsCharacter::CanUnSheathe()
{
	return(ActionState == EActionState::EAS_Unoccupied && CharState == ECharacterState::ECS_Unequipped && EquippedWeapon);
}

void ASwordsCharacter::Unequip() {
	PlayEquipMontage(FName("Sheathe"));
	CharState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_Equip;
}

void ASwordsCharacter::Equip()
{
	PlayEquipMontage(FName("Draw"));
	CharState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_Equip;
}

void ASwordsCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void ASwordsCharacter::AttachWeaponToHip()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket_Lower"));
	}
}

void ASwordsCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("hand_r_socket"));
	}
}

void ASwordsCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SheatheMontage) {
		AnimInstance->Montage_Play(SheatheMontage);
		AnimInstance->Montage_JumpToSection(SectionName, SheatheMontage);
	}
}

void ASwordsCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

bool ASwordsCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool ASwordsCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ASwordsCharacter::EndEquip()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASwordsCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASwordsCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASwordsCharacter::InitializeSwordsOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController()); 
	if (PlayerController)
	{
		ASwordHUD* SwordHUD = Cast<ASwordHUD>(PlayerController->GetHUD());
		if (SwordHUD)
		{
			SwordsOverlay = SwordHUD->GetSwordsOverlay();
			if (SwordsOverlay && Attributes)
			{
				SwordsOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SwordsOverlay->SetStaminaBarPercent(1.f);
				SwordsOverlay->SetGold(0);
				SwordsOverlay->SetSouls(0);
			}
		}
	}
}

void ASwordsCharacter::SetHUDHealth()
{
	if (SwordsOverlay && Attributes)
	{
		UE_LOG(LogTemp, Warning, TEXT("In if of SetHUDHealth"));
		SwordsOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}
