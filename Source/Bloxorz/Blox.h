// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "BloxGridTile.h"
#include "Blox.generated.h"

class ABloxorzGameModeBase;

UENUM(BlueprintType)
enum BloxState
{
	XFLAT    UMETA(DisplayName = "XFLAT"),
	YFLAT    UMETA(DisplayName = "YFLAT"),
	STANDING UMETA(DisplayName = "STANDING"),
	SPLIT     UMETA(DisplayName = "SPLIT")
};

enum BloxBehaviour
{
	DEFAULT_STATE UMETA(DisplayName = "Static"),
	ANIMATION_STATE UMETA(DisplayName = "AnimationMode"),
	HIGHLIGHT_STATE UMETA(DisplayName = "HighlightMode"),
	START_STATE	UMETA(DisplayName = "StartState"),
	FALL_STATE UMETA(DisplayName = "FallState"),
	END_STATE UMETA(DisplayName  "EndState"),
	DEAD_STATE UMETA(DisplayName = "DeadState")
};

enum BloxMoveDirection
{
	BLOX_LEFT	UMETA(DisplayName = "LEFT"),
	BLOX_RIGHT   UMETA(DisplayName = "RIGHT"),
	BLOX_UP		UMETA(DisplayName = "UP"),
	BLOX_DOWN	UMETA(DisplayName = "DOWN")
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveTrigger);
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTrigger);
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathTrigger);

UCLASS()
class BLOXORZ_API ABlox : public APawn
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Split Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SplitAction;

public:
	// Sets default values for this pawn's properties
	ABlox();

	BloxState GetBloxState();
	void SetBloxState(BloxState NewState);
	void SetBloxBehaviour(BloxBehaviour Behaviour);
	UFUNCTION(BlueprintImplementableEvent)
	void OnMove();

	// Blueprint events used for level transitions
	UFUNCTION(BlueprintImplementableEvent)
	void OnEnd();



	UFUNCTION()
	void StartEndAnimation(ABloxGridTile* Tile);

	void Fall();

	void ToggleVisibility(bool bIsVisible);

	void SnapToGrid();

	UFUNCTION()
	void Highlight();

	void Rotate(BloxMoveDirection DirectionToRotate);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);

	void TransitionTo(BloxMoveDirection MoveDirection, FQuat& NewRotation);

	void Split(const FInputActionValue& Value);

	bool LineTrace(FVector Start, FVector End, ABloxGridTile* TileHit);

	void PostMove();

	void ProcessStart();

	void ProcessFall();

	void ProcessAnimation();

	void ProcessHighlight();

	void ProcessEnd();

	UFUNCTION()
	void StartAnimation();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayMoveSound(BloxState CurrentState);


	
	void StartBurnAnimation(ABloxGridTile* Tile);
	void PlayBurnAnimation();
	void StartStandardEndAnimation(ABloxGridTile* Tile);
	void PlayStandardEndAnimation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	BloxBehaviour BehaviourState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bToggleSplitState = false;
	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationLength = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EndAnimationLength = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FOnMoveTrigger OnMoveTrigger;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FOnEndTrigger OnEndTrigger;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FOnDeathTrigger OnDeathTrigger;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EndAnimationFallSpeed = 2.0f;

	float StartHeight = 1000;
	float GroundLevel = 0.0f;
	bool TriggerStart = false;
	float AnimationStartTime;
	bool InAnimation = false;
	bool InEndState = false;
	bool IsValid = true;
	bool InStartState = false;
	bool PlayStartSound = true;
	bool FallAnimation = false;
	FQuat StartRotation;
	FQuat EndRotation;
	FVector  StartLocation;
	FVector  EndLocation;

	// End Animation Information
	bool bQueueEndState = false;
	UPROPERTY(EditAnywhere, Category = "EndAnimation")
	UMaterialInterface* BoxMaterial;
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	UPROPERTY(EditAnywhere, Category = "EndAnimation")
	float FallOffStart = 50;
	UPROPERTY(EditAnywhere, Category = "EndAnimation")
	float FallOffEnd = 300;
	UPROPERTY(EditAnywhere, Category = "EndAnimation")
	float Bound = 30;
	UPROPERTY(EditAnywhere, Category = "EndAnimation")
	float Glow = 10;

	BloxState State = STANDING;
	BloxMoveDirection PreviousMoveDirection;
	float CubeScale = 100.0f;
	float TimeOfPreviousMove = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InputDelay = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* BloxPivot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RectangleMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* CameraPivot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* OverlayMaterial;

	const FRotator RightRotation = FRotator(-90, 0, 0);
	const FRotator LeftRotation = FRotator(90, 0, 0);
	const FRotator UpRotation = FRotator(0, 0, -90);
	const FRotator DownRotation = FRotator(0, 0, 90);

	ABloxorzGameModeBase* GameModeBase;

};
