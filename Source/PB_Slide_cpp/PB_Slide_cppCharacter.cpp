// Copyright Epic Games, Inc. All Rights Reserved.

#include "PB_Slide_cppCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APB_Slide_cppCharacter

APB_Slide_cppCharacter::APB_Slide_cppCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//----------------------------------------------------------------------------------------------------
	//											MY STUFF START
	//----------------------------------------------------------------------------------------------------
	WalkSpeed = 600.0f;
	SprintSpeed = 1000.0f;
	CrouchSpeed = 300.0f;

	GetCapsuleComponent()->SetHiddenInGame(false);
	StandingCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	bIsSprintKeyDown = false;
	bIsCrouchKeyDown = false;
	//----------------------------------------------------------------------------------------------------
	//											 MY STUFF END
	//----------------------------------------------------------------------------------------------------
}

void APB_Slide_cppCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APB_Slide_cppCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((PlayerMovementStateENUM == EPlayerMovementState::Crouching) && (bIsCrouchKeyDown == false))
	{
		if (CanStand())
		{
			ResolveMovementState();
		}
	}
}

void APB_Slide_cppCharacter::SprintKeyPressed()
{
	bIsSprintKeyDown = true;
	if (PlayerMovementStateENUM == EPlayerMovementState::Walking || PlayerMovementStateENUM == EPlayerMovementState::Crouching)
	ResolveMovementState();
}

void APB_Slide_cppCharacter::SprintKeyReleased()
{
	bIsSprintKeyDown = false;
	if (PlayerMovementStateENUM == EPlayerMovementState::Running)
	{
		ResolveMovementState();
	}
}

void APB_Slide_cppCharacter::CrouchKeyPressed()
{
	bIsCrouchKeyDown = true;
	UCharacterMovementComponent* PlayerCharacterMovementComponent = GetCharacterMovement();
	if ((PlayerMovementStateENUM == EPlayerMovementState::Walking || PlayerMovementStateENUM == EPlayerMovementState::Running) && !PlayerCharacterMovementComponent->IsFalling())
	SetPlayerMovementState( (bIsSprintKeyDown && GetVelocity().Length() >= 750.0f) ? EPlayerMovementState::Sliding : EPlayerMovementState::Crouching);
}

void APB_Slide_cppCharacter::CrouchKeyReleased()
{
	bIsCrouchKeyDown = false;
	if (PlayerMovementStateENUM != EPlayerMovementState::Sliding)
	{
		ResolveMovementState();
	}
}

void APB_Slide_cppCharacter::ResolveMovementState()
{
	EPlayerMovementState ResultState;

	ResultState = (CanStand()) ? EPlayerMovementState::Walking : EPlayerMovementState::Crouching;
	ResultState = (CanSprint()) ? EPlayerMovementState::Running : ResultState;

	SetPlayerMovementState(ResultState);
}

void APB_Slide_cppCharacter::SetPlayerMovementState(EPlayerMovementState NewPlayerMovementState)
{
	if (PlayerMovementStateENUM == NewPlayerMovementState)
	{
		return;
	}

	EPlayerMovementState PrevPlayerMovementState = PlayerMovementStateENUM;
	PlayerMovementStateENUM = NewPlayerMovementState;

	OnPlayerMovementStateChange(PrevPlayerMovementState);

	switch (PlayerMovementStateENUM)
	{
	case EPlayerMovementState::Walking:
		StopCrouch();
		break;

	case EPlayerMovementState::Running:
		StopCrouch();
		break;

	case EPlayerMovementState::Crouching:
		StartCrouch();
		break;

	case EPlayerMovementState::Sliding:
		StartCrouch();
		StartSliding();
		break;
	}
}

void APB_Slide_cppCharacter::OnPlayerMovementStateChange(EPlayerMovementState PrevPlayerMovementState)
{
	UCharacterMovementComponent* PlayerCharacterMovementComponent = GetCharacterMovement();

	switch(PlayerMovementStateENUM)
	{
	case EPlayerMovementState::Walking:
		PlayerCharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
		break;

	case EPlayerMovementState::Running:
		PlayerCharacterMovementComponent->MaxWalkSpeed = SprintSpeed;
		break;

	case EPlayerMovementState::Crouching:
		PlayerCharacterMovementComponent->MaxWalkSpeed = CrouchSpeed;
		break;

	case EPlayerMovementState::Sliding:
		PlayerCharacterMovementComponent->MaxWalkSpeed = 0.0f;
		break;
	}

	if (PrevPlayerMovementState == EPlayerMovementState::Sliding)
	{
		PlayerCharacterMovementComponent->BrakingDecelerationWalking = 2000.0f;
	}

	if (PlayerMovementStateENUM == EPlayerMovementState::Sliding)
	{
		PlayerCharacterMovementComponent->Velocity = GetActorForwardVector() * SprintSpeed * 1.5f;
	}
}

bool APB_Slide_cppCharacter::CanStand() const
{
	if (bIsCrouchKeyDown)
	{
		return false;
	}

	FVector CapsuleBottomPoint = GetActorLocation();
	CapsuleBottomPoint.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector CapsuleTopPoint = CapsuleBottomPoint;
	CapsuleTopPoint.Z += 2 * StandingCapsuleHalfHeight;

	DrawDebugSphere(this->GetWorld(), CapsuleBottomPoint, 8.0f, 8, FColor::Black, false, 0.0f);
	DrawDebugSphere(this->GetWorld(), CapsuleTopPoint, 8.0f, 8, FColor::White, false, 0.0f);

	FHitResult HitRes;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	bool bDoesLineTraceHit = GetWorld()->LineTraceSingleByChannel(HitRes, CapsuleBottomPoint, CapsuleTopPoint, ECollisionChannel::ECC_Visibility, CollisionParams);

	return (bDoesLineTraceHit) ? false : true;
}

bool APB_Slide_cppCharacter::CanSprint() const
{
	if (!bIsSprintKeyDown)
	{
		return false;
	}

	return (CanStand()) ? true : false;
}

void APB_Slide_cppCharacter::FloorInfluenceFunction()
{
	UCharacterMovementComponent* PlayerCharacterMovementComponent = GetCharacterMovement();
	FFindFloorResult FloorResult = PlayerCharacterMovementComponent->CurrentFloor;
	FHitResult FloorHitResult = FloorResult.HitResult;

	if (FloorHitResult.ImpactNormal == GetActorUpVector())
	{
		return;
	}

	FVector ForceToApplyNormalized = FVector::CrossProduct(FloorHitResult.ImpactNormal, FVector::CrossProduct(FloorHitResult.ImpactNormal, GetActorUpVector()));
	PlayerCharacterMovementComponent->AddForce(ForceToApplyNormalized * 7000000.0f);
}

void APB_Slide_cppCharacter::IsSlideSpeedSlowerThanCrouchSpeed()
{
	UCharacterMovementComponent* PlayerCharacterMovementComponent = GetCharacterMovement();
	if (GetVelocity().Length() <= CrouchSpeed)
	{
		ResolveMovementState();
		StopSliding();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APB_Slide_cppCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APB_Slide_cppCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APB_Slide_cppCharacter::Look);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APB_Slide_cppCharacter::SprintKeyPressed);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APB_Slide_cppCharacter::SprintKeyReleased);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APB_Slide_cppCharacter::CrouchKeyPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APB_Slide_cppCharacter::CrouchKeyReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APB_Slide_cppCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	if (PlayerMovementStateENUM == EPlayerMovementState::Sliding)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APB_Slide_cppCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}