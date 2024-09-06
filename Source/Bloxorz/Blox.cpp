// Fill out your copyright notice in the Description page of Project Settings.


#include "Blox.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BloxGrid.h"
#include "BloxorzGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include "BloxGameInstance.h"

// Sets default values
ABlox::ABlox()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	CameraPivot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPivot"));
	BloxPivot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BloxPivot"));
	RectangleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rectangle"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	BloxPivot->SetupAttachment(RootComponent);
	CameraPivot->SetupAttachment(RootComponent);
	Camera->SetupAttachment(CameraPivot);
	RectangleMesh->SetupAttachment(BloxPivot);
}

BloxState ABlox::GetBloxState()
{
	return State;
}

void ABlox::SetBloxState(BloxState NewState)
{
	State = NewState;
}

void ABlox::SetBloxBehaviour(BloxBehaviour Behaviour)
{
	BehaviourState = Behaviour;
}

void ABlox::StartEndAnimation(ABloxGridTile* Tile)
{
	// Old end animation
	// StartBurnAnimation(Tile);
	StartStandardEndAnimation(Tile);
}

void ABlox::StartBurnAnimation(ABloxGridTile* Tile)
{
	EBloxTileType TileType = Tile->TileType;
	if (State != STANDING)
		return;

	BehaviourState = BloxBehaviour::END_STATE;
	IsValid = false;
	AnimationStartTime = GetWorld()->TimeSeconds;
	StartLocation = GetActorLocation();
	EndLocation = StartLocation - FVector(0, 0, 300);

	if (DynamicMaterialInstance)
	{
		RectangleMesh->SetCastShadow(false);
		DynamicMaterialInstance->SetScalarParameterValue("EdgeFallof", FallOffStart);
		DynamicMaterialInstance->SetScalarParameterValue("Bounds", Bound);
		DynamicMaterialInstance->SetScalarParameterValue("Glow", Glow);
	}
	bQueueEndState = false;
	SetActorTickEnabled(true);
}

void ABlox::StartStandardEndAnimation(ABloxGridTile* Tile)
{
	EBloxTileType TileType = Tile->TileType;
	if (State != STANDING)
		return;

	BehaviourState = BloxBehaviour::END_STATE;
	IsValid = false;
	AnimationStartTime = GetWorld()->TimeSeconds;
	StartLocation = GetActorLocation();
	EndLocation = StartLocation - FVector(0, 0, 650);

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue("HeightMask", 200);
		DynamicMaterialInstance->SetScalarParameterValue("HeightPosition", -200);
	}
	bQueueEndState = false;
	SetActorTickEnabled(true);
}

void ABlox::PlayStandardEndAnimation()
{
	// End animation
	float AnimationTimePassed = GetWorld()->TimeSeconds - AnimationStartTime;
	float Alpha = AnimationTimePassed / EndAnimationLength;
	FVector  CurrentLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	float HeightMaskValue = FMath::Lerp(200, 1200, Alpha);
	SetActorLocation(CurrentLocation);
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue("HeightMask", HeightMaskValue);
	}
	// Animation ends
	if (AnimationTimePassed > EndAnimationLength)
	{
		BehaviourState = BloxBehaviour::DEFAULT_STATE;
		RectangleMesh->SetVisibility(false);
		SetActorTickEnabled(false);
		// Call blueprint event to trigger level transition
		OnEndTrigger.Broadcast();
	}
}

void ABlox::PlayBurnAnimation()
{
	// End animation
	float AnimationTimePassed = GetWorld()->TimeSeconds - AnimationStartTime;
	// Interpolate current position and rotation
	float Alpha = AnimationTimePassed / EndAnimationLength;
	float EdgeFallOfValue = FMath::Lerp(FallOffStart, FallOffEnd, Alpha);
	if (DynamicMaterialInstance)
		DynamicMaterialInstance->SetScalarParameterValue("EdgeFallof", EdgeFallOfValue);


	// Animation ends
	if (AnimationTimePassed > EndAnimationLength)
	{
		BehaviourState = BloxBehaviour::DEFAULT_STATE;
		RectangleMesh->SetVisibility(false);
		SetActorTickEnabled(false);
		// Call blueprint event to trigger level transition
		OnEndTrigger.Broadcast();
	}
}

void ABlox::Fall()
{
	BehaviourState = BloxBehaviour::FALL_STATE;
	AnimationStartTime = GetWorld()->TimeSeconds;
	StartLocation = GetActorLocation();
	EndLocation = StartLocation + (FVector::DownVector * 500);
	SetActorTickEnabled(true);
	//RectangleMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//RectangleMesh->SetSimulatePhysics(true);
	//RectangleMesh->AddImpulse(FVector::DownVector);
}

void ABlox::ToggleVisibility(bool bIsVisible)
{
	RectangleMesh->SetVisibility(bIsVisible);
}

void ABlox::SnapToGrid()
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + (FVector::DownVector * 10000),
		ECC_Visibility
	);
	FVector NewLocation = GetActorLocation();
	if (State == SPLIT)
		NewLocation.Z = HitResult.Location.Z + (CubeScale / 2);
	else
		NewLocation.Z = HitResult.Location.Z + (CubeScale + CubeScale / 4);

	SetActorLocation(NewLocation);
}

void ABlox::Highlight()
{
	if (!OverlayMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid overlay material"));
		return;
	}
	BehaviourState = BloxBehaviour::HIGHLIGHT_STATE;
	RectangleMesh->SetOverlayMaterial(OverlayMaterial);
	AnimationStartTime = GetWorld()->TimeSeconds;
}

void ABlox::Rotate(BloxMoveDirection DirectionToRotate)
{
	FVector2D MovementVector;
	switch (DirectionToRotate)
	{
	case BLOX_UP:
		BloxPivot->AddWorldRotation(UpRotation * -1);
		MovementVector = FVector2D(0.f, 1.f);
		State = YFLAT;
		UE_LOG(LogTemp, Warning, TEXT("UP"));
		break;
	case BLOX_DOWN:
		BloxPivot->AddWorldRotation(DownRotation * -1);
		MovementVector = FVector2D(0.f, -1.f);
		State = YFLAT;
		UE_LOG(LogTemp, Warning, TEXT("DOWN"));
		break;
	case BLOX_RIGHT:
		BloxPivot->AddWorldRotation(RightRotation);
		MovementVector = FVector2D(-1.f, 0.f);
		State = XFLAT;
		UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
		break;
	case BLOX_LEFT:
		BloxPivot->AddWorldRotation(LeftRotation);
		MovementVector = FVector2D(1.f, 0.f);
		State = XFLAT;
		UE_LOG(LogTemp, Warning, TEXT("LEFT"));
		break;
	}
	FVector NewLocation = GetActorLocation() + FVector(MovementVector * (float)((CubeScale / 2)), -(float)(CubeScale / 2));
	SetActorLocation(NewLocation);
}

// Called when the game starts or when spawned
void ABlox::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	TimeOfPreviousMove = -100;

	// Set Materials
	if (BoxMaterial)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BoxMaterial, this);
		if (DynamicMaterialInstance)
		{
			RectangleMesh->SetMaterial(0, DynamicMaterialInstance);
			DynamicMaterialInstance->SetScalarParameterValue("HeightMask", 0);
			DynamicMaterialInstance->SetScalarParameterValue("HeightPosition", 0);
			DynamicMaterialInstance->SetScalarParameterValue("Glow", 0);
		}
	}

	// Snap to grid start if it exists
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABloxGrid::StaticClass(), FoundActors);
	if (FoundActors.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find level grid"));
		return;
	}
	ABloxGrid* Grid = Cast<ABloxGrid>(FoundActors[0]);
	Grid->OnAnimationEndTrigger.AddDynamic(this, &ABlox::StartAnimation);
	RectangleMesh->SetVisibility(false);
	BloxPivot->SetWorldRotation(FRotator::ZeroRotator);

	if (bToggleSplitState)
	{
		UE_LOG(LogTemp, Warning, TEXT("SPLIT STATE SCALE"));
		State = BloxState::SPLIT;
		FVector NewLocation = GetActorLocation() + FVector(0.0f, 0.0f, -(CubeScale / 2));
		SetActorLocation(NewLocation);

	}

	GameModeBase = Cast<ABloxorzGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameModeBase)
	{
		OnEndTrigger.AddDynamic(GameModeBase, &ABloxorzGameModeBase::OnLevelEnd);
		OnDeathTrigger.AddDynamic(GameModeBase, &ABloxorzGameModeBase::OnDeath);
	}
	// Get a reference to the game instance
	UBloxGameInstance* BloxGameInstance = Cast<UBloxGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (BloxGameInstance)
	{
		OnMoveTrigger.AddDynamic(BloxGameInstance, &UBloxGameInstance::IncrementMoveDelta);
	}

	BehaviourState = START_STATE;
	SetActorTickEnabled(false);
}

// Called every frame
void ABlox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (BehaviourState)
	{
	case START_STATE:
		ProcessStart();
		break;
	case FALL_STATE:
		ProcessFall();
		break;
	case ANIMATION_STATE:
		ProcessAnimation();
		break;
	case HIGHLIGHT_STATE:
		ProcessHighlight();
		break;
	case END_STATE:
		ProcessEnd();
		break;
	case DEAD_STATE:
		OnDeathTrigger.Broadcast();
		SetActorTickEnabled(false);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("DEFAULT"));
		SetActorTickEnabled(false);
		break;
	}
}

// Called to bind functionality to input
void ABlox::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlox::Move);
		//Shoot
		EnhancedInputComponent->BindAction(SplitAction, ETriggerEvent::Triggered, this, &ABlox::Split);
	}
}

void ABlox::Move(const FInputActionValue& Value)
{
	if (!IsValid || GetWorld()->TimeSeconds - TimeOfPreviousMove < InputDelay || BehaviourState != DEFAULT_STATE)
	{
		return;
	}
	if (!BloxPivot)
	{
		UE_LOG(LogTemp, Warning, TEXT("Null Blux Pivot"));
	}

	TimeOfPreviousMove = GetWorld()->TimeSeconds;
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.X != 0 && MovementVector.Y != 0)
	{
		return;
	}
	FVector NewLocation;
	FQuat NewRotation = BloxPivot->GetComponentQuat();
	MovementVector.Y *= -1;
	// Compute new actor location
	// Since the blox is falling and rotating the movement has to be scaled accordingly
	switch (State)
	{
	case STANDING:
		NewLocation = GetActorLocation() + FVector(MovementVector * ((CubeScale / 2) + CubeScale), -(CubeScale / 2));
		if (MovementVector.X != 0)  // LEFT RIGHT
		{
			MovementVector.X > 0 ? TransitionTo(BLOX_RIGHT, NewRotation) : TransitionTo(BLOX_LEFT, NewRotation);
		}
		else if (MovementVector.Y != 0) // UP DOWN
		{
			MovementVector.Y > 0 ? TransitionTo(BLOX_UP, NewRotation) : TransitionTo(BLOX_DOWN, NewRotation);
		}
		break;

	case XFLAT:
		if (MovementVector.X != 0)  // LEFT RIGHT
		{
			NewLocation = GetActorLocation() + FVector(MovementVector * ((CubeScale / 2) + CubeScale), (CubeScale / 2));
			MovementVector.X > 0 ? TransitionTo(BLOX_RIGHT, NewRotation) : TransitionTo(BLOX_LEFT, NewRotation);
		}
		else if (MovementVector.Y != 0) // UP DOWN
		{
			NewLocation = GetActorLocation() + FVector(MovementVector * (CubeScale), 0.0f);
			MovementVector.Y > 0 ? TransitionTo(BLOX_DOWN, NewRotation) : TransitionTo(BLOX_UP, NewRotation);
		}
		break;

	case YFLAT:
		if (MovementVector.X != 0)  // LEFT RIGHT
		{
			NewLocation = GetActorLocation() + FVector(MovementVector * (CubeScale), 0);
			MovementVector.X > 0 ? TransitionTo(BLOX_RIGHT, NewRotation) : TransitionTo(BLOX_LEFT, NewRotation);
		}
		else if (MovementVector.Y != 0) // UP DOWN
		{
			NewLocation = GetActorLocation() + FVector(MovementVector * (CubeScale + (CubeScale / 2)), (CubeScale / 2));
			MovementVector.Y > 0 ? TransitionTo(BLOX_DOWN, NewRotation) : TransitionTo(BLOX_UP, NewRotation);
		}
		break;
	case SPLIT:
		NewLocation = GetActorLocation() + FVector(MovementVector * (CubeScale), 0);
		if (MovementVector.X != 0)  // LEFT RIGHT
		{
			MovementVector.X > 0 ? TransitionTo(BLOX_RIGHT, NewRotation) : TransitionTo(BLOX_LEFT, NewRotation);
		}
		else if (MovementVector.Y != 0) // UP DOWN
		{
			MovementVector.Y > 0 ? TransitionTo(BLOX_UP, NewRotation) : TransitionTo(BLOX_DOWN, NewRotation);
		}
		break;


	default:
		break;
	}

	BehaviourState = BloxBehaviour::ANIMATION_STATE;
	AnimationStartTime = GetWorld()->TimeSeconds;
	//StartRotation = BloxPivot->GetComponentQuat();
	EndRotation = NewRotation;
	StartLocation = GetActorLocation();
	EndLocation = NewLocation;
	PlayMoveSound(State);
	SetActorTickEnabled(true);

	// Call blueprint event to increment global move counter
	OnMoveTrigger.Broadcast();
}


void ABlox::TransitionTo(BloxMoveDirection MoveDirection, FQuat& NewRotation)
{
	StartRotation = BloxPivot->GetComponentQuat();
	switch (State)
	{
	case SPLIT:
		// Behaves the same as standing
		switch (MoveDirection)
		{
		case BLOX_UP:
			BloxPivot->AddWorldRotation(UpRotation * -1);
			NewRotation = BloxPivot->GetComponentQuat();
			break;
		case BLOX_DOWN:
			BloxPivot->AddWorldRotation(DownRotation * -1);
			NewRotation = BloxPivot->GetComponentQuat();
			break;
		case BLOX_RIGHT:
			BloxPivot->AddWorldRotation(RightRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			break;
		case BLOX_LEFT:
			BloxPivot->AddWorldRotation(LeftRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			break;
		}
		break;
	case STANDING:
		// Transition from a standing state
		switch (MoveDirection)
		{
		case BLOX_UP:
			BloxPivot->AddWorldRotation(UpRotation * -1);
			NewRotation = BloxPivot->GetComponentQuat();
			State = YFLAT;
			break;
		case BLOX_DOWN:
			BloxPivot->AddWorldRotation(DownRotation * -1);
			NewRotation = BloxPivot->GetComponentQuat();
			State = YFLAT;
			break;
		case BLOX_RIGHT:
			BloxPivot->AddWorldRotation(RightRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = XFLAT;
			break;
		case BLOX_LEFT:
			BloxPivot->AddWorldRotation(LeftRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = XFLAT;
			break;
		}
		break;

	case XFLAT:
		// Transition from a flat state along the x axis
		switch (MoveDirection)
		{
		case BLOX_RIGHT:
			BloxPivot->AddWorldRotation(RightRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = STANDING;
			break;
		case BLOX_LEFT:
			BloxPivot->AddWorldRotation(LeftRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = STANDING;
			break;
		case BLOX_UP:
			BloxPivot->AddWorldRotation(UpRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = XFLAT;
			break;
		case BLOX_DOWN:
			BloxPivot->AddWorldRotation(DownRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = XFLAT;
			break;
		}
		break;

	case YFLAT:
		switch (MoveDirection)
		{
		case BLOX_RIGHT:
			BloxPivot->AddWorldRotation(RightRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = YFLAT;
			break;
		case BLOX_LEFT:
			BloxPivot->AddWorldRotation(LeftRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = YFLAT;
			break;
		case BLOX_UP:
			BloxPivot->AddWorldRotation(UpRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = STANDING;
			break;
		case BLOX_DOWN:
			BloxPivot->AddWorldRotation(DownRotation);
			NewRotation = BloxPivot->GetComponentQuat();
			State = STANDING;
			break;
		}
		break;
		// Transition from a flat state along the y

	default:
		break;
	}

	PreviousMoveDirection = MoveDirection;
	BloxPivot->SetWorldRotation(StartRotation);
}

void ABlox::Split(const FInputActionValue& Value)
{
	if (State != BloxState::SPLIT)
		return;
	UE_LOG(LogTemp, Warning, TEXT("Split "));
	if (!GameModeBase)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABlox::Split Unable to access game mode base"));
		return;
	}
	GameModeBase->SwitchBloxPawn();
	// Get Game Mode Base
}

bool ABlox::LineTrace(FVector Start, FVector End, ABloxGridTile* TileHit)
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	FHitResult HitResult;

	FVector TraceStartLocation = Start;
	FVector TraceEndLocation = End;
	bool bHit = GetWorld()->LineTraceSingleByChannel
	(
		HitResult,
		TraceStartLocation,
		TraceEndLocation,
		ECC_Visibility,
		CollisionParams
	);
	if (!bHit)
		return bHit;
	TileHit = Cast<ABloxGridTile>(HitResult.GetActor());
	if (!TileHit)  return bHit;
	TileHit->Trigger(this);
	return bHit;
}

void ABlox::PostMove()
{
	// Check if box is in the grid
	// Emit a line trace to check for a tile below
	FVector TraceStartLocation;
	FVector TraceEndLocation;
	FVector StartLocationOne;
	FVector EndLocationOne;
	FVector StartLocationTwo;
	FVector EndLocationTwo;
	ABloxGridTile* CurrentTile = nullptr;
	bool bHitFaceOne;
	bool bHitFaceTwo;
	bool bHit;
	switch (State)
	{
	case SPLIT:
		// Check if split blox moved to a valid tile
		TraceStartLocation = GetActorLocation();
		TraceEndLocation = TraceStartLocation + (FVector::DownVector * 2 * CubeScale);
		bHit = LineTrace(TraceStartLocation, TraceEndLocation, CurrentTile);
		if (!bHit)
		{
			RectangleMesh->SetSimulatePhysics(true);
			IsValid = false;
			BehaviourState = DEAD_STATE;
			return;
		}
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Red, true);

		// Check if the split boxes need to merge
		// TopFace
		TraceStartLocation = GetActorLocation();
		TraceEndLocation = TraceStartLocation + (FVector::LeftVector * 1 * CubeScale);
		bHit = LineTrace(TraceStartLocation, TraceEndLocation, CurrentTile);
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Yellow, true);
		if (bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("BLOX RIGHT"));
			GameModeBase->MergeSplitBlox(BloxMoveDirection::BLOX_DOWN);
			SetActorTickEnabled(false);
			return;
		}

		// BotFace
		TraceEndLocation = TraceStartLocation + (FVector::RightVector * 1 * CubeScale);
		bHit = LineTrace(TraceStartLocation, TraceEndLocation, CurrentTile);
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Green, true);
		if (bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("BLOX LEFT"));
			GameModeBase->MergeSplitBlox(BloxMoveDirection::BLOX_UP);
			SetActorTickEnabled(false);
			return;
		}

		// LeftFace
		TraceEndLocation = TraceStartLocation + (FVector::BackwardVector * 1 * CubeScale);
		bHit = LineTrace(TraceStartLocation, TraceEndLocation, CurrentTile);
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Red, true);
		if (bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("BLOX UP"));
			GameModeBase->MergeSplitBlox(BloxMoveDirection::BLOX_RIGHT);
			SetActorTickEnabled(false);
			return;
		}

		// RightFace
		TraceEndLocation = TraceStartLocation + (FVector::ForwardVector * 1 * CubeScale);
		bHit = LineTrace(TraceStartLocation, TraceEndLocation, CurrentTile);
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Blue, true);
		if (bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("BLOX DOWN"));
			GameModeBase->MergeSplitBlox(BloxMoveDirection::BLOX_LEFT);
			SetActorTickEnabled(false);
			return;
		}

	case STANDING:
		TraceStartLocation = GetActorLocation();
		TraceEndLocation = TraceStartLocation + (FVector::DownVector * 2 * CubeScale);
		bHit = LineTrace(TraceStartLocation, TraceEndLocation, CurrentTile);
		if (!bHit)
		{
			RectangleMesh->SetSimulatePhysics(true);
			IsValid = false;
			BehaviourState = DEAD_STATE;
		}
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Red,true);
		break;
	case XFLAT:
		StartLocationOne = GetActorLocation() + FVector(CubeScale / 2, 0, 0);
		EndLocationOne = StartLocationOne - FVector(0, 0, CubeScale * 2);
		StartLocationTwo = GetActorLocation() + FVector(-CubeScale / 2, 0, 0);
		EndLocationTwo = StartLocationTwo - FVector(0, 0, CubeScale * 2);
		bHitFaceOne = LineTrace(StartLocationOne, EndLocationOne, CurrentTile);
		bHitFaceTwo = LineTrace(StartLocationTwo, EndLocationTwo, CurrentTile);
		if (!bHitFaceOne || !bHitFaceTwo)
		{
			RectangleMesh->SetSimulatePhysics(true);
			IsValid = false;
			BehaviourState = DEAD_STATE;
		}
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Green, true);
		break;
	case YFLAT:
		StartLocationOne = GetActorLocation() + FVector(0, CubeScale / 2, 0);
		EndLocationOne = StartLocationOne - FVector(0, 0, CubeScale * 2);
		StartLocationTwo = GetActorLocation() + FVector(0, -CubeScale / 2, 0);
		EndLocationTwo = StartLocationTwo - FVector(0, 0, CubeScale * 2);
		bHitFaceOne = LineTrace(StartLocationOne, EndLocationOne, CurrentTile);
		bHitFaceTwo = LineTrace(StartLocationTwo, EndLocationTwo, CurrentTile);
		if (!bHitFaceOne || !bHitFaceTwo)
		{
			RectangleMesh->SetSimulatePhysics(true);
			IsValid = false;
			BehaviourState = DEAD_STATE;
		}
		//DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Blue, true);
		break;
	}
}

void ABlox::ProcessStart()
{
	if (!TriggerStart) return;
	if (PlayStartSound)
	{
		PlayMoveSound(STANDING);
		UE_LOG(LogTemp, Warning, TEXT("END ANIMATION"));
		PlayStartSound = false;
	}
	float AnimationTimePassed = GetWorld()->TimeSeconds - AnimationStartTime;
	float Alpha = AnimationTimePassed / AnimationLength;
	FVector  CurrentLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	SetActorLocation(CurrentLocation);
	// Animation ends
	if (AnimationTimePassed > AnimationLength)
	{
		SetActorLocation(EndLocation);
		BehaviourState = BloxBehaviour::DEFAULT_STATE;
		return;
	}
}

void ABlox::ProcessFall()
{
	float AnimationTimePassed = GetWorld()->TimeSeconds - AnimationStartTime;
	float Alpha = AnimationTimePassed / AnimationLength;
	FVector  CurrentLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	SetActorLocation(CurrentLocation);
	// Animation ends
	if (AnimationTimePassed > AnimationLength)
	{
		RectangleMesh->SetSimulatePhysics(true);
		BehaviourState = DEAD_STATE;
		return;
	}
}

void ABlox::ProcessAnimation()
{
	// Movement Animation
	float AnimationTimePassed = GetWorld()->TimeSeconds - AnimationStartTime;
	if (AnimationTimePassed > AnimationLength)
	{
		// Animation ends
		StartRotation = EndRotation;
		SetActorLocation(EndLocation);
		BloxPivot->SetWorldRotation(EndRotation);
		BehaviourState = BloxBehaviour::DEFAULT_STATE;
		PostMove();
		return;
	}

	// Called when in animation
	// Interpolate current position and rotation
	float Alpha = AnimationTimePassed / AnimationLength;
	FQuat CurrentRotation = FQuat::FastLerp(StartRotation, EndRotation, Alpha);
	FVector  CurrentLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	SetActorLocation(CurrentLocation);
	BloxPivot->SetWorldRotation(CurrentRotation);
}

void ABlox::ProcessHighlight()
{
	float AnimationTimePassed = GetWorld()->TimeSeconds - AnimationStartTime;
	// Highlight ends
	const float HIGHLIGHT_LENGTH = 0.25;
	if (AnimationTimePassed > HIGHLIGHT_LENGTH)
	{
		BehaviourState = BloxBehaviour::DEFAULT_STATE;
		RectangleMesh->SetOverlayMaterial(false);
		return;
	}
}

void ABlox::ProcessEnd()
{
	PlayStandardEndAnimation();
}

void ABlox::StartAnimation()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Animation Called"));

	// Snap to grid start if it exists
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABloxGrid::StaticClass(), FoundActors);
	if (FoundActors.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find level grid"));
		return;
	}

	ABloxGrid* Grid = Cast<ABloxGrid>(FoundActors[0]);
	FVector PlayerStartLocation;
	if (bToggleSplitState)
		PlayerStartLocation = Grid->GetPlayerStart() + FVector(0, 0, CubeScale * 0.75);
	else
		PlayerStartLocation = Grid->GetPlayerStart() + FVector(0, 0, CubeScale + (CubeScale / 4));
	GroundLevel = PlayerStartLocation.Z;
	StartLocation = PlayerStartLocation;
	StartLocation.Z += StartHeight;
	EndLocation = PlayerStartLocation;
	SetActorLocation(StartLocation);
	RectangleMesh->SetVisibility(true);
	AnimationStartTime = GetWorld()->TimeSeconds;
	BehaviourState = BloxBehaviour::START_STATE;
	SetActorTickEnabled(true);
	TriggerStart = true;
}

