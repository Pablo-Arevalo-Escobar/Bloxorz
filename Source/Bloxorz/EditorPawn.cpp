// Fill out your copyright notice in the Description page of Project Settings.


#include "EditorPawn.h"
#include "Components/ComboBox.h"
#include "EditorWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


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




// Sets default values
AEditorPawn::AEditorPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



// Called when the game starts or when spawned
void AEditorPawn::BeginPlay()
{
	Super::BeginPlay();

	// Initialize editor state
	EditorState = EEditorState::EDITING;

	//Add Input Mapping Context
	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EditorMappingContext, 0);
		}
		PlayerController->SetShowMouseCursor(true);
		PlayerController->bEnableMouseOverEvents = true;
	}


	// Retrieve grid
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABloxGrid::StaticClass(), FoundActors);
	if (FoundActors.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find level grid"));
		return;
	}
	Grid = Cast<ABloxGrid>(FoundActors[0]);
	if (!Grid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Grid Reference"));
	}


	// Initalize values
	SelectedTileIndex = -1;
	SelectedTileName = "";

    // Initalize combo boxT
    if(!EditorWidget)
    {
        UE_LOG(LogTemp,Warning,TEXT("Editor widget template null"));
        return;
    }
    Widget = CreateWidget<UEditorWidget>(GetWorld(), EditorWidget);
    if(!Widget)
    {
        UE_LOG(LogTemp,Warning,TEXT("Invalid widget reference"));
        return;
    }
    Widget->AddToViewport();
	// Get the enum class pointer
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EBloxTileType"), true);
	if (EnumPtr)
	{
		// Get the name of the enum value
	    for(EBloxTileType ETType : 	TypeList)
        {
            FString EnumName = EnumPtr->GetNameStringByValue((int64)ETType);
            Widget->TileTypeUsing->AddOption(EnumName);
        }
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
	Widget->TileTypeUsing->SetSelectedIndex(0);
    Widget->ResolutionSpinBox->SetValue(10);
	Widget->GridResolutionButton->OnPressed.AddDynamic(this, &AEditorPawn::ResizeGrid);
    Widget->FillButton->OnPressed.AddDynamic(this, &AEditorPawn::FillGrid);
    Widget->EmptyButton->OnPressed.AddDynamic(this, &AEditorPawn::EmptyGrid);
	Widget->SerializeButton->OnPressed.AddDynamic(this, &AEditorPawn::SerializeGrid);
}

// Called every frame
void AEditorPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEditorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &AEditorPawn::OnMouseMoved);
		EnhancedInputComponent->BindAction(MouseClickedAction, ETriggerEvent::Triggered, this, &AEditorPawn::OnMouseClicked);
		EnhancedInputComponent->BindAction(SwitchTileTypeAction, ETriggerEvent::Triggered, this, &AEditorPawn::SwitchTileType);
		EnhancedInputComponent->BindAction(DeleteTileAction, ETriggerEvent::Triggered, this, &AEditorPawn::OnDeleteAction);
		EnhancedInputComponent->BindAction(ChangeModeAction, ETriggerEvent::Triggered, this, &AEditorPawn::ChangeEditorMode);
	}
}

// Called when player mouse is moved
void AEditorPawn::OnMouseMoved(const FInputActionValue& Value)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Player Controller"));
		return;
	}
}

void AEditorPawn::OnMouseClicked(const FInputActionValue& Value)
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


	switch (EditorState) {
	case EEditorState::LINKING:

		// Store switch tile index
		if (TileHit->TileType == EBloxTileType::BUTTON_SWITCH || TileHit->TileType == EBloxTileType::CROSS_SWITCH)
		{
			// Unhighlight previously selected tiles

			// Highlight current tiles
			LinkSendTileIndex = TileHit->TileIndex;
            TileHit->HighlightTile(true);
		}

		else if (TileHit->TileType == EBloxTileType::BRIDGE_LEFT || TileHit->TileType == EBloxTileType::BRIDGE_RIGHT
				|| TileHit->TileType == EBloxTileType::BRIDGE_UP || TileHit->TileType == EBloxTileType::BRIDGE_DOWN)
		{
			if (!LinkMap.Contains(LinkSendTileIndex))
			{
				TSet<int> LinkReceiveTileIndex;
				LinkReceiveTileIndex.Add(TileHit->TileIndex);
				LinkMap.Add(LinkSendTileIndex, LinkReceiveTileIndex);
			}
			else
			{
				LinkMap.Find(LinkSendTileIndex)->Add(TileHit->TileIndex);
			}
            TileHit->HighlightTile(true);
		}
		break;

	case EEditorState::EDITING:

		// Actor is in Editing state
		// If actor is empty at the current tile type
		if (TileHit->TileType == EBloxTileType::EMPTY)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tile Empty, Change"));
			Grid->SwitchTileType(SelectedTileIndex, TypeList[TileTypeIndex]);
		}
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
		Grid->SwitchTileType(SelectedTileIndex, EBloxTileType::EMPTY);
		break;
	}

}

void AEditorPawn::SwitchTileType(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Switch Tile"));
	if (SelectedTileIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Valid Tile Selected"));
		return;
	}
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
	Grid->SwitchTileType(SelectedTileIndex, TypeList[TileTypeIndex]);
	Widget->TileTypeUsing->SetSelectedIndex(TileTypeIndex);


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

	Grid->SerializeGrid();
}

void AEditorPawn::ChangeEditorMode(const FInputActionValue& Value)
{
	switch (EditorState)
	{
	case EEditorState::EDITING:
		EditorState = EEditorState::LINKING;
		break;
	case EEditorState::LINKING:
		EditorState = EEditorState::EDITING;
		break;
	}
}

void AEditorPawn::SerializeGrid()
{
	Grid->SerializeGrid();
	if (LinkMap.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Serialize LinkMap IsEmpty"));
		return;
	}
	for (auto& KeyValPair : LinkMap)
	{
		int LinkCaller  = KeyValPair.Key;
		for (int LinkReceiver : KeyValPair.Value)
		{
			UE_LOG(LogTemp, Warning, TEXT("%d-%d"), LinkCaller, LinkReceiver);
		}
	}
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
	SetCameraLocation(CameraLocation);
}

void AEditorPawn::FillGrid()
{
    if(!Grid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid reference"));
        return;
    }
	UE_LOG(LogTemp, Warning, TEXT("Calling FILL"));
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
