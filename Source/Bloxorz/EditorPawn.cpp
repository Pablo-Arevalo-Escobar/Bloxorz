// Fill out your copyright notice in the Description page of Project Settings.
#include "EditorPawn.h"
#include "Components/ComboBox.h"
#include "EditorWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include <BloxFileManager/Public/BloxFileManager.h>


// CONSTANTS
static const TArray<EBloxTileType> TypeList
{
	EBloxTileType::STANDARD		    ,
	EBloxTileType::START			,
	EBloxTileType::END				,
	EBloxTileType::FALL			    ,
	EBloxTileType::BUTTON_SWITCH	,
	EBloxTileType::CROSS_SWITCH	    ,
	EBloxTileType::BRIDGE_LEFT		,
	EBloxTileType::BRIDGE_RIGHT		,
	EBloxTileType::BRIDGE_UP		,
	EBloxTileType::BRIDGE_DOWN		,
	EBloxTileType::EMPTY			,
	EBloxTileType::SPLIT			,
};

AEditorPawn::AEditorPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(RootComponent);
	Tags.Add(TEXT("EditorPawn"));
}

void AEditorPawn::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessAI = EAutoPossessAI::Disabled;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	// Initialize state and input info
	EditorState = EEditorState::EDITING;
	SelectedTileIndex = -1;
	SelectedTileName = "";

	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EditorMappingContext, 0);
		}
	}

	// RETRIEVE GRID AND INIT FOR EDITING
	// NOTE :: Map editor expects a grid to be present in the level
	//
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABloxGrid::StaticClass(), FoundActors);
	if (FoundActors.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find level grid"));
		return;
	}
	ensureMsgf((Grid = Cast<ABloxGrid>(FoundActors[0])) != nullptr, TEXT("EditorPawn::Invalid Grid Reference"));

	if (SceneCaptureComponent)
	{
		SceneCaptureComponent->SetAbsolute(true, true, true);
		SceneCaptureComponent->SetRelativeLocation(FVector(10000, 10000, 0));
		SceneCaptureComponent->SetRelativeRotation(FRotator(-90, 180, 0));

		PreviewTileLocation = (SceneCaptureComponent->GetForwardVector() * PreviewTileDistance) + SceneCaptureComponent->GetRelativeLocation();
	}

	// LOG SIZE OF AACTOR, COMPONENT, BLOXGRIDTILE, BLOXGRID
	UE_LOG(LogTemp, Warning, TEXT("Size of AActor: %d"), sizeof(AActor));
	UE_LOG(LogTemp, Warning, TEXT("Size of UActorComponent: %d"), sizeof(UActorComponent));
	UE_LOG(LogTemp, Warning, TEXT("Size of ABloxGridTile: %d"), sizeof(ABloxGridTile));
	UE_LOG(LogTemp, Warning, TEXT("Size of ABloxGrid: %d"), sizeof(ABloxGrid));

	// RETRIEVE CAMERA
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("LevelCamera"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		CameraActor = Cast<AActor>(FoundActors[0]);
		PlayerController->SetViewTarget(CameraActor);
	}

    // INITIALIZE WIDGET
    if(!EditorWidget)
    {
        UE_LOG(LogTemp,Warning,TEXT("Editor widget template null"));
        return;
    }

	ensureMsgf(
		(Widget = CreateWidget<UEditorWidget>(GetWorld(), EditorWidget)) != nullptr, 
		TEXT("Invalid widget reference")
	);
    Widget->AddToViewport();


	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBloxTileType"), true);
	if (EnumPtr)
	{
		// Get the name of the enum value
	    for (EBloxTileType ETType : TypeList)
        {
            FString EnumName = EnumPtr->GetNameStringByValue((int64)ETType);
            Widget->TileTypeUsing->AddOption(EnumName);
        }

#if WITH_EDITOR
		FString EnumName = EnumPtr->GetNameStringByValue((int64)TypeList[TileTypeIndex]);
		SelectedTileName = EnumName;
		// Create the message
		FString Message = FString::Printf(TEXT("Enum Index %d, Enum value: %s"), TileTypeIndex,  *EnumName);

		// Print the message to the screen
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
		}
#endif

	}

	Widget->TileTypeUsing->SetSelectedIndex(0);
    Widget->ResolutionSpinBox->SetValue(10);
	Widget->GridResolutionButton->OnPressed.AddDynamic(this, &AEditorPawn::ResizeGrid);
    Widget->FillButton->OnPressed.AddDynamic(this, &AEditorPawn::FillGrid);
    Widget->EmptyButton->OnPressed.AddDynamic(this, &AEditorPawn::EmptyGrid);
	Widget->SerializeButton->OnPressed.AddDynamic(this, &AEditorPawn::SerializeGrid);
}
	
void AEditorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Draw link lines
	const float LinkEditHeight = 50.0f;
	if (EditorState == EEditorState::LINKING)
    {
		for (auto& KeyValPair : LinkMap)
		{
			FVector StartLocation = Grid->GetTileLocation(KeyValPair.Key);
			FColor LineColor = KeyValPair.Key == LinkSendTileIndex? FColor::Green : FColor::Red;
			for (int LinkReceiver : KeyValPair.Value)
			{
				FVector EndLocation = Grid->GetTileLocation(LinkReceiver);
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, LineColor, false, -1, SDPG_Foreground, 5);
			}
		}
    }

	// Rotate preview tile
	if (PreviewTile)
	{
		PreviewTile->SetActorRotation(FRotator(PreviewTileRotation.X, PreviewTileRotation.Y, PreviewTileRotation.Z));

		const float UpperBound = 65.0f;
		const float LowerBound = 25.0f;
		if (PreviewTileRotation.X > UpperBound)
		{
			PreviewTileRotation.X = UpperBound;
			sign *= -1;
		}
		else if(PreviewTileRotation.X < LowerBound)
        {
            PreviewTileRotation.X = LowerBound;
            sign *= -1;
        }
		PreviewTileRotation.X = PreviewTileRotation.X > UpperBound ? LowerBound : PreviewTileRotation.X + (0.05f * sign);
    }
}

void AEditorPawn::PossessedBy(AController* NewController)
{
	PlayerController = Cast<APlayerController>(NewController);
	if (!CameraActor || !NewController) return;
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("LevelCamera"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		CameraActor = Cast<AActor>(FoundActors[0]);
		PlayerController->SetViewTarget(CameraActor);
	}
	UE_LOG(LogTemp, Warning, TEXT("Possessed by EditorPawn"));
}

// Called to bind functionality to input
void AEditorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MouseClickedAction, ETriggerEvent::Triggered, this, &AEditorPawn::OnMouseClicked);
		EnhancedInputComponent->BindAction(SwitchTileTypeAction, ETriggerEvent::Triggered, this, &AEditorPawn::SwitchTileType);
		EnhancedInputComponent->BindAction(DeleteTileAction, ETriggerEvent::Triggered, this, &AEditorPawn::OnDeleteAction);
		EnhancedInputComponent->BindAction(ChangeModeAction, ETriggerEvent::Triggered, this, &AEditorPawn::ChangeEditorMode);
		EnhancedInputComponent->BindAction(EmptyGridAction, ETriggerEvent::Triggered, this, &AEditorPawn::EmptyGrid);
		EnhancedInputComponent->BindAction(FillGridAction, ETriggerEvent::Triggered, this, &AEditorPawn::FillGrid);
	}
}

void AEditorPawn::OnMouseClicked(const FInputActionValue& Value)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Player Controller"));
		return;
	}

	// ATTEMPT TO GET SELECTED TILE AND UPDATE ACCORDINGLY
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (!HitResult.bBlockingHit || !HitResult.GetActor())
	{
		return;
	}

	ABloxGridTile* TileHit = Cast<ABloxGridTile>(HitResult.GetActor());
	if (!TileHit)
	{
		return;
	}
	SelectedTileIndex = TileHit->TileIndex;

	switch (EditorState) {
	case EEditorState::LINKING:
		LinkTile(*TileHit);
		break;

	case EEditorState::EDITING:
		if ((PrevSelectedTileIndex == SelectedTileIndex && Grid->GetTileType(SelectedTileIndex) == TypeList[TileTypeIndex])
			|| TypeList[TileTypeIndex] == EBloxTileType::START && StartTilePlaced || TypeList[TileTypeIndex] == EBloxTileType::END && EndTilePlaced)
		{
			break;
		}
		if (TypeList[TileTypeIndex] == EBloxTileType::START)
        {
			StartTilePlaced = true;
        }
		else if (TypeList[TileTypeIndex] == EBloxTileType::END)
		{
			EndTilePlaced = true;
		}
		if (Grid->GetTileType(SelectedTileIndex) == EBloxTileType::START)
        {
            StartTilePlaced = false;
        }
        else if (Grid->GetTileType(SelectedTileIndex) == EBloxTileType::END)
        {
            EndTilePlaced = false;
        }
		Grid->SwitchTileType(SelectedTileIndex, TypeList[TileTypeIndex]);
		PrevSelectedTileIndex = SelectedTileIndex;
		break;

	default:
		checkNoEntry();
        break;
	}

}

void AEditorPawn::LinkTile(ABloxGridTile& TileHit)
{
	switch (TileHit.TileType)
	{
		// Store switch tile index
	case EBloxTileType::SPLIT:
		LinkSendTileIsSplit = true;
		AddToLinkMap(TileHit);
		break;
	case EBloxTileType::CROSS_SWITCH:
	case EBloxTileType::BUTTON_SWITCH:
		LinkSendTileIsSplit = false;
		AddToLinkMap(TileHit);
		break;

	// Only allow linking to bridge tiles
	case EBloxTileType::BRIDGE_LEFT:
	case EBloxTileType::BRIDGE_RIGHT:
	case EBloxTileType::BRIDGE_UP:
	case EBloxTileType::BRIDGE_DOWN:
		if (LinkSendTileIndex == -1)
            return;
		if (!LinkMap.Contains(LinkSendTileIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("LinkSendTileIndex not found in LinkMap"));
			break;
		}

		if (LinkMap.Find(LinkSendTileIndex)->Num() >= 2 && LinkSendTileIsSplit)
		{
			break;
		}
		LinkMap.Find(LinkSendTileIndex)->Add(TileHit.TileIndex);
		TileHit.HighlightTile(true);
		
		break;

	case EBloxTileType::EMPTY:
	case EBloxTileType::END:
		break;
	default:
		if (!(LinkSendTileIsSplit && LinkMap.Find(LinkSendTileIndex)->Num() < 2) || LinkSendTileIndex == -1 || !LinkMap.Contains(LinkSendTileIndex)) return;
		LinkMap.Find(LinkSendTileIndex)->Add(TileHit.TileIndex);
		TileHit.HighlightTile(true);
		break;

	}
}

void AEditorPawn::OnDeleteAction(const FInputActionValue& Value)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Player Controller"));
		return;
	}

	// Get actor under cursor
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (!HitResult.bBlockingHit || !HitResult.GetActor())
	{
		return;
	}

	ABloxGridTile* TileHit = Cast<ABloxGridTile>(HitResult.GetActor());
	if (!TileHit)
	{
		return;
	}
	SelectedTileIndex = TileHit->TileIndex;

	switch (EditorState)
	{
	case EEditorState::LINKING:
		if (!LinkMap.Contains(LinkSendTileIndex) || !LinkMap[LinkSendTileIndex].Contains(TileHit->TileIndex))
			return;
		LinkMap[LinkSendTileIndex].Remove(TileHit->TileIndex);
        TileHit->HighlightTile(false);
		break;
	case EEditorState::EDITING:
		// Highlight actor if under cursor
		if (LinkMap.Contains(TileHit->TileIndex))
		{
			LinkMap.Remove(TileHit->TileIndex);
		}
		if (TileHit->TileType == EBloxTileType::START)
        {
            StartTilePlaced = false;
        }
        else if (TileHit->TileType == EBloxTileType::END)
        {
            EndTilePlaced = false;
        }
		Grid->SwitchTileType(SelectedTileIndex, EBloxTileType::EMPTY);
		break;
	}

}

void AEditorPawn::SwitchTileType(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Switch Tile"));
	if (!Grid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid reference to grid"));
		return;
	}

	int TileTypeIndexIncrement = Value.Get<FVector>().X;
	TileTypeIndex += TileTypeIndexIncrement;

	// Switch Grid Tile Type
	if (TileTypeIndex >= TypeList.Num())
	{
		TileTypeIndex = 0;
	}
	else if (TileTypeIndex < 0)
	{
		TileTypeIndex = TypeList.Num() - 1;
	}
	Widget->TileTypeUsing->SetSelectedIndex(TileTypeIndex);
	if (PreviewTile)
	{
        PreviewTile->Destroy();
    }
	PreviewTileLocation = (SceneCaptureComponent->GetForwardVector() * PreviewTileDistance) + SceneCaptureComponent->GetRelativeLocation();
	PreviewTile = Grid->MakeTile(TypeList[TileTypeIndex]);
	PreviewTile->SetActorLocation(PreviewTileLocation);

#if WITH_EDITOR
	// Get the enum class pointer
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBloxTileType"), true);
	if (EnumPtr)
	{
		// Get the name of the enum value
		
		FString EnumName = EnumPtr->GetNameStringByValue((int64)TypeList[TileTypeIndex]);
		SelectedTileName = EnumName;
		// Create the message
		FString Message = FString::Printf(TEXT("Enum Index %d, Enum value: %s"), TileTypeIndex,  *EnumName);

		// Print the message to the screen
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
		}
	}
#endif
}

void AEditorPawn::ChangeEditorMode(const FInputActionValue& Value)
{
	switch (EditorState)
	{
	case EEditorState::EDITING:
		EditorState = EEditorState::LINKING;
		VerifyLinkIntegrity();
		break;
	case EEditorState::LINKING:
		EditorState = EEditorState::EDITING;
		StopLinkVisualization();
		break;
	}
}

void AEditorPawn::SerializeGrid()
{
	FBloxGridData wGridData;
	wGridData.mCameraTransform = SceneCaptureComponent->GetComponentTransform();
	wGridData.mGridResolution = Grid->GetGridResolution();
	wGridData.mGridTiles = Grid->GetGridTileTypes();
	wGridData.mLinkMap = LinkMap;
	wGridData.mGridName = "CustomLevel";

	BloxFileManager::Serialize(wGridData);
}

FString AEditorPawn::GetSelectedTile()
{
	return SelectedTileName;
}

void AEditorPawn::ResizeGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("RESIZE GRID %f"), Widget->ResolutionSpinBox->GetValue());
    if(!Grid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Grid Reference"));
        return;
    }

	LinkMap.Empty();
    Grid->Resize((int)Widget->ResolutionSpinBox->GetValue() + 1);
	
	// Update camera location
	FVector CameraLocation = Grid->GetActorLocation() + (FVector::UpVector * Widget->ResolutionSpinBox->GetValue() * 150) + (FVector::RightVector * Widget->ResolutionSpinBox->GetValue() * 50);
	if (CameraActor) CameraActor->SetActorLocation(CameraLocation);
}

void AEditorPawn::FillGrid()
{
    if(!Grid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid reference"));
        return;
    }
	if (TypeList[TileTypeIndex] == EBloxTileType::START || TypeList[TileTypeIndex] == EBloxTileType::END) return; 
    Grid->Fill(TypeList[TileTypeIndex]);
}

void AEditorPawn::EmptyGrid()
{
    if(!Grid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid reference"));
        return;
    }
	UE_LOG(LogTemp, Warning, TEXT("Calling FILL EMPTY"));
	LinkMap.Empty();
    Grid->Fill(EBloxTileType::EMPTY);
}

void AEditorPawn::AddToLinkMap(ABloxGridTile& TileHit)
{
	// Unhighlight previously selected tiles
	StopLinkVisualization();

	// Highlight currently selected tiles
	LinkSendTileIndex = TileHit.TileIndex;
	TileHit.HighlightTile(true);
	if (LinkMap.Contains(LinkSendTileIndex))
	{
		for (int LinkReceiver : LinkMap[LinkSendTileIndex])
		{
			Grid->HighlightTileAtIndex(LinkReceiver, true);
		}
	}
	else
	{
		TSet<int> LinkReceiveTileIndex;
		LinkMap.Add(LinkSendTileIndex, LinkReceiveTileIndex);
	}
}

void AEditorPawn::VerifyLinkIntegrity()
{
	// Unhighlight previously selected tiles
	for (auto& KeyValPair : LinkMap)
	{
		bool IsValidSendTile = Grid->GetTileType(KeyValPair.Key) == EBloxTileType::BUTTON_SWITCH
			|| Grid->GetTileType(KeyValPair.Key) == EBloxTileType::CROSS_SWITCH
			|| Grid->GetTileType(KeyValPair.Key) == EBloxTileType::SPLIT;

		if (!IsValidSendTile)
		{
			Grid->HighlightTileAtIndex(KeyValPair.Key, false);
            LinkMap.Remove(KeyValPair.Key);
            continue;
		}
		for (int LinkReceiver : KeyValPair.Value)
		{
			bool IsValidReceiveTile = Grid->GetTileType(LinkReceiver) != EBloxTileType::END || Grid->GetTileType(LinkReceiver) == EBloxTileType::EMPTY;
			if (!IsValidReceiveTile)
			{
				Grid->HighlightTileAtIndex(LinkReceiver, false);
                LinkMap[KeyValPair.Key].Remove(LinkReceiver);
			}
		}
	}
	LinkSendTileIndex = -1;
}

void AEditorPawn::StopLinkVisualization()
{
	// Unhighlight previously selected tiles
	for (auto& KeyValPair : LinkMap)
	{
		Grid->HighlightTileAtIndex(KeyValPair.Key, false);
		for (int LinkReceiver : KeyValPair.Value)
		{
			Grid->HighlightTileAtIndex(LinkReceiver, false);
		}
	}
}
