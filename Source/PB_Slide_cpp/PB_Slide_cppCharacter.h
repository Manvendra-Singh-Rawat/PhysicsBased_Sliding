// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlayerMovementState_ENUM.h"
#include "DrawDebugHelpers.h"
#include "PB_Slide_cppCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APB_Slide_cppCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	APB_Slide_cppCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//------------------------------------------------------------------------------------------------------------
	//											MY STUFF START
	//------------------------------------------------------------------------------------------------------------

	// VARIABLES----------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Constants")
	float WalkSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Constants")
	float SprintSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Constants")
	float CrouchSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float StandingCapsuleHalfHeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	bool bIsSprintKeyDown;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	bool bIsCrouchKeyDown;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	EPlayerMovementState PlayerMovementStateENUM;

	// FUNCTIONS----------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "MovementFunctions")
	void SprintKeyPressed();
	UFUNCTION(BlueprintCallable, Category = "MovementFunctions")
	void SprintKeyReleased();

	UFUNCTION(BlueprintCallable, Category = "MovementFunctions")
	void CrouchKeyPressed();
	UFUNCTION(BlueprintCallable, Category = "MovementFunctions")
	void CrouchKeyReleased();

	UFUNCTION(BlueprintCallable, Category = "MovementFunctions")
	void ResolveMovementState();

	UFUNCTION(BlueprintCallable, Category = "MovementFunctions")
	void SetPlayerMovementState(EPlayerMovementState NewPlayerMovementState);

	UFUNCTION(BlueprintCallable, Category = "MovementFunctions")
	void OnPlayerMovementStateChange(EPlayerMovementState PrevPlayerMovementState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MovementChecks")
	bool CanStand() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MovementChecks")
	bool CanSprint() const;

	//------------------------------------------------------------------------------------------------------------
	//											 MY STUFF END
	//------------------------------------------------------------------------------------------------------------
};

