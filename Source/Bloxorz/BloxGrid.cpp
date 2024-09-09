// Fill out your copyright notice in the Description page of Project Settings.


#include "BloxGrid.h"
#include "BridgeTile.h"
#include <Kismet/GameplayStatics.h>
#include "Blox.h"
#include "SplitTile.h"
#include "BloxorzGameModeBase.h"

#include <BloxFileManager/Public/BloxFileManager.h>


// Sets default values
ABloxGrid::ABloxGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE"));
}

ABloxGrid::~ABloxGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("Destructor Called"));
}

// Called when the game starts or when spawned
void ABloxGrid::BeginPlay()
{
	Super::BeginPlay();
	//if (!LoadOnStart)
	//	return;
	Initalize();
	PlayStartAnimation();
}

void ABloxGrid::OnConstruction(const FTransform& Transform)
{
	UE_LOG(LogTemp, Warning, TEXT("OnConstruction"));
	Super::PostActorCreated();
	if (!CreateNew || HasAllFlags(RF_Transient))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid has already been constructed"));
		return;
	}
	if (!FloorArray.IsEmpty())
	{
		FloorArray.Empty();
		UE_LOG(LogTemp, Warning, TEXT("Emptying Array"));
	}
	FString LevelToLoad = Level + ".bloxlevel";
	UE_LOG(LogTemp, Warning, TEXT("Grid Construction"));
	LoadLevel2(LevelToLoad);
	CreateNew = false;
}
void ABloxGrid::Reconstruct()
{
	FString LevelToLoad = Level + ".bloxlevel";
	UE_LOG(LogTemp, Warning, TEXT("Grid Construction"));
	Initalize();
}
void ABloxGrid::Initalize()
{
	// Verify that all subclasses are passed in
	if (!Tile || !EndTile || !ButtonSwitchTile || !CrossSwitchTile || !BridgeTile || !EmptyTile || !FallTile || !StartTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("A tile subclass is not initialized"));
		SetActorTickEnabled(false);
		return;
	}

	// Cleanup any tiles created in the editor 
	TArray<AActor*> Tiles;
	GetAttachedActors(Tiles);
	if (!Tiles.IsEmpty())
	{
		for (AActor* GridTile : Tiles)
		{
			GridTile->Destroy();
		}
        Tiles.Empty();
	}
	if (!ValidFloors.IsEmpty())
	{
		ValidFloors.Empty();
	}

	if (InEditorMode)
	{
		LoadEmpty();
		PlayStartAnimation();
	}
	else 
	{
		// Load level
		FString LevelToLoad = Level;
		LoadLevel2(LevelToLoad);
	}
	
}

void ABloxGrid::LoadLevel2(FString iLevelToLoad)
{
	bool wIsValidGridSetup = !Tile || !EndTile || !ButtonSwitchTile || !CrossSwitchTile || !BridgeTile || !EmptyTile || !FallTile || !StartTile;
	if (wIsValidGridSetup)
	{
		UE_LOG(LogTemp, Warning, TEXT("A tile subclass is not initialized"));
		SetActorTickEnabled(false);
		return;
	}
	CleanUpGrid();

	FBloxGridData wGridData = BloxFileManager::Parse(iLevelToLoad);
	if (wGridData.mGridResolution == -1) {UE_LOG(LogTemp, Error, TEXT("Invalid file format")); return;}
	if (wGridData.mGridResolution * wGridData.mGridResolution < wGridData.mGridTiles.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load level. Grid tile size does not match denoted resolution. Res : %d   TileSize %d"), (wGridData.mGridResolution*wGridData.mGridResolution), wGridData.mGridTiles.Num());
		return;
	}
			
	// CREATE AND LOAD THE TILES INTO THE GRIDS FLOOR ARRAY
	FVector GridStart(-(GridCellSize * GridResolution) / 2, (GridCellSize * GridResolution) / 2, 0.0f);
	int NumberOfTiles = wGridData.mGridTiles.Num();
	FloorArray.Init(nullptr, NumberOfTiles);
	for (int wIndex = 0; wIndex < wGridData.mGridTiles.Num(); ++wIndex)
	{
		int32 col = wIndex % wGridData.mGridResolution;
		int32 row = wIndex / wGridData.mGridResolution;
		FVector wTileRelativeLocation = GridStart + FVector((GridCellSize * row), -(GridCellSize * col), 0.f);
		FloorArray[wIndex] = MakeTileInternal(wTileRelativeLocation, wIndex, wGridData.mGridTiles[wIndex], true);
		if (FloorArray[wIndex]) ValidFloors.Add(wIndex);
	}

	// TODO :: INITIALIZE CAMERA POSITION

	// CREATE AND LOAD THE LINKS
	for (auto& wKeyValue : wGridData.mLinkMap)
	{
		int wTriggerTileIndex = wKeyValue.Key;
		check(wTriggerTileIndex < FloorArray.Num() && wTriggerTileIndex >= 0);
		for (int wReceiveTileIndex : wKeyValue.Value)
		{
			check(wReceiveTileIndex < FloorArray.Num() && wReceiveTileIndex >= 0);
			if (FloorArray[wTriggerTileIndex] == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Trigger tile ptr used for link is null"));
				return;
			}
			switch (FloorArray[wTriggerTileIndex]->TileType)
			{
			case EBloxTileType::SPLIT:
				// Set spawn tile index to receive tile index
				Cast<ASplitTile>(FloorArray[wTriggerTileIndex])->SetSpawnTileIndex(wReceiveTileIndex);
				break;

			case EBloxTileType::BUTTON_SWITCH:
			case EBloxTileType::CROSS_SWITCH:
				FloorArray[wTriggerTileIndex]->TileTrigger.AddDynamic(FloorArray[wReceiveTileIndex], &ABloxGridTile::LinkReceived);
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("Invalid Trigger Tile Encountered When Loading Link Map: %d %d"), wTriggerTileIndex, static_cast<int>(FloorArray[wTriggerTileIndex]->TileType));
				break;
			}
		}
	}
}

void ABloxGrid::LoadLevel(FString LevelToLoad, bool IsPreview)
{
	//BloxFileManager::Parse(LevelToLoad);


	// Verify that all subclasses are passed in
	if (!Tile || !EndTile || !ButtonSwitchTile || !CrossSwitchTile || !BridgeTile || !EmptyTile || !FallTile || !StartTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("A tile subclass is not initialized"));
		SetActorTickEnabled(false);
		return;
	}

	// Cleanup any tiles created in the editor 
	TArray<AActor*> Tiles;
	GetAttachedActors(Tiles);
	if (!Tiles.IsEmpty())
	{
		for (AActor* GridTile : Tiles)
		{
			GridTile->Destroy();
		}
		Tiles.Empty();
	}
	if (!ValidFloors.IsEmpty())
	{
		ValidFloors.Empty();
	}

	// Read data from file
	FString RelativePath = FPaths::ProjectContentDir().Append("/Maps/MapFiles/").Append(LevelToLoad).Append(".bloxlevel");
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*RelativePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Level load failed. Grid file does not exist. File name %s"), *RelativePath);
		return;
	}

	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *RelativePath, FFileHelper::EHashOptions::None))
	{
		UE_LOG(LogTemp, Warning, TEXT("Level file exists but cannot be loaded to string"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Text from file %s"), *FileContent);
	
	// Process text
	TArray<FString> GridLines;
	TArray<FString> LinkLines;
	ReadIntoLines(FileContent, GridLines, LinkLines);

	// Create Grid
	GridResolution = FCString::Atoi(*GridLines[0]);
	int NumberOfTiles = GridResolution * GridResolution;
	FloorArray.Init(nullptr, NumberOfTiles);
	FVector GridStart(-(GridCellSize * GridResolution) / 2, (GridCellSize * GridResolution) / 2, 0.0f);
	bool ValidTiles = true;
	for (int i = 1; i < GridLines.Num(); ++i)
	{
		FString& Line = GridLines[i];
		if (Line.IsEmpty()) continue;
		for (int j = 0; j < GridResolution; ++j)
		{
			if (j >= Line.Len()) break;
			int TileIndex = ((i - 1) * GridResolution) + j;
			if (TileIndex >= NumberOfTiles)
			{
				UE_LOG(LogTemp, Warning, TEXT("Level Format Invalid"));
				ValidTiles = false;
				break;
			}
			FVector TileRelativeLocation = GridStart + FVector((GridCellSize * i), -(GridCellSize * j), 0.f);
			
			// Tile type creation
			switch(Line[j])
			{
			case 's':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::STANDARD, true);
				ValidFloors.Add(TileIndex);
				break;
			case 'b':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::START, true);
				PlayerStartTileIndex = TileIndex;
				ValidFloors.Add(TileIndex);
				break;
			case 'h':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::END, true);
				ValidFloors.Add(TileIndex);
				break;
			case 'q':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::BUTTON_SWITCH, true);
				ValidFloors.Add(TileIndex);
				break;
			case 'e':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::CROSS_SWITCH, true);
				ValidFloors.Add(TileIndex);
				break;
			case 'f':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::FALL, true);
				ValidFloors.Add(TileIndex);
				break;
			case  '*':
				// Empty tiles are only created in the level editor for visualization purposes
				// In game the pointers are left null
				if (InEditorMode)
				{
					FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::EMPTY, true);
					ValidFloors.Add(TileIndex);
				}
				break;
			// Bridge type tiles
			case 'i':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::BRIDGE_LEFT, true);
				ValidFloors.Add(TileIndex);
				break;
			case 'p':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::BRIDGE_RIGHT, true);
				ValidFloors.Add(TileIndex);
				break;
			case 'o':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::BRIDGE_UP, true);
				ValidFloors.Add(TileIndex);
				break;
			case 'l':
				FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::BRIDGE_DOWN, true);
				ValidFloors.Add(TileIndex);
				break;
            case 'k':
                FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::SPLIT, true);
                ValidFloors.Add(TileIndex);
			default:
				break;
			}
		}
		if (!ValidTiles) break;
	}

	// Load links between tiles
	for (int i = 0; i < LinkLines.Num(); ++i)
	{
		FString& Line = LinkLines[i];
		if (Line.IsEmpty()) continue;
		FString TriggerTileIndexS;
		FString ReceiveTileIndexS;

		// Create links between tiles by subscribing the tile on the right to the event creator on the left
		// TXT file for links is in the format of
		// TriggerTile-ReceiverTile
		const char Seperator = '-';
		int LineIndex = 0;
		bool SeperatorEncountered = false;
		TCHAR CurrChar = '!';
		while (CurrChar != '\n' && LineIndex < Line.Len())
		{
			CurrChar = Line[LineIndex];
			if (CurrChar == Seperator)
			{
				SeperatorEncountered = true;
				++LineIndex;
				CurrChar = Line[LineIndex];
				continue;
			}
			if (!SeperatorEncountered)
			{
				TriggerTileIndexS.AppendChar(CurrChar);
			}
			else
			{
				ReceiveTileIndexS.AppendChar(CurrChar);
			}
			++LineIndex;
		}

		int TriggerTileIndex = FCString::Atoi(*TriggerTileIndexS);
		int ReceiveTileIndex = FCString::Atoi(*ReceiveTileIndexS);
		if (TriggerTileIndex < 0 || TriggerTileIndex > FloorArray.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("INVALID TRIGGER TILE INDEX IN LINKS"));
			return;
		}
		if (ReceiveTileIndex < 0 || ReceiveTileIndex > FloorArray.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("INVALID RECEIVE TILE INDEX IN LINKS"));
			return;
		}
		if (FloorArray[TriggerTileIndex] == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("NULL REFERENCE TO LINKED TILE"));
			return;
		}

		switch (FloorArray[TriggerTileIndex]->TileType)
		{
		case EBloxTileType::SPLIT:
			// Set spawn tile index to receive tile index
			Cast<ASplitTile>(FloorArray[TriggerTileIndex])->SetSpawnTileIndex(ReceiveTileIndex);
			break;
		
		case EBloxTileType::BUTTON_SWITCH:
		case EBloxTileType::CROSS_SWITCH:
			FloorArray[TriggerTileIndex]->TileTrigger.AddDynamic(FloorArray[ReceiveTileIndex], &ABloxGridTile::LinkReceived);
            break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Game State: %d %d"), TriggerTileIndex, static_cast<int>(FloorArray[TriggerTileIndex]->TileType));
			return;
			//checkNoEntry();
			break;
		}
	}
	PlayStartAnimation();
}

void ABloxGrid::LoadEmpty()
{
	int NumberOfTiles = GridResolution * GridResolution;
	FloorArray.Init(nullptr, NumberOfTiles);
	FVector GridStart(-(GridCellSize * GridResolution) / 2, (GridCellSize * GridResolution) / 2, 0.0f);
	bool ValidTiles = true;
	for (int i = 1; i < GridResolution; ++i)
	{
		for (int j = 0; j < GridResolution; ++j)
		{
			int TileIndex = ((i - 1) * GridResolution) + j;
			if (TileIndex >= NumberOfTiles)
			{
				UE_LOG(LogTemp, Warning, TEXT("Level Format Invalid"));
				ValidTiles = false;
				break;
			}
			FVector TileRelativeLocation = GridStart + FVector((GridCellSize * i), -(GridCellSize * j), 0.f);
			FloorArray[TileIndex] = MakeTileInternal(TileRelativeLocation, TileIndex, EBloxTileType::EMPTY, true);
			ValidFloors.Add(TileIndex);
		}
	}
	PlayerStartTileIndex = 0;
}

void ABloxGrid::PlayStartAnimation()
{
    UE_LOG(LogTemp,Warning, TEXT("BloxGrid::PlayStartAnimation"));
	if (ValidFloors.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Valid Floors"));
		return;
	}
	InAnimation = true;
	AnimationStartTime = GetWorld()->TimeSeconds;
	SetActorTickEnabled(true);

	// Offset tiles for effect
	for (int Index : ValidFloors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Index %d"), Index);
		FVector FloorLocation = FloorArray[Index]->GetActorLocation();
		FRandomStream RandStream(FMath::Rand());
		RandStream.GenerateNewSeed();
		float RandValue = RandStream.FRandRange(0, StartAnimationRandOffset);
		FloorZOffset.Add(Index, RandValue);
		FloorLocation.Z -= RandValue;
		FloorArray[Index]->SetActorLocation(FloorLocation);
	}
}

void ABloxGrid::CleanUpGrid()
{
	TArray<AActor*> Tiles;
	GetAttachedActors(Tiles);
	if (!Tiles.IsEmpty())
	{
		for (AActor* GridTile : Tiles)
		{
			GridTile->Destroy();
		}
		Tiles.Empty();
	}
	if (!ValidFloors.IsEmpty())
	{
		ValidFloors.Empty();
	}
}

// Called every frame
void ABloxGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!InAnimation)
	{
		this->SetActorTickEnabled(false);
		return;
	}
	if ( (GetWorld()->TimeSeconds - AnimationStartTime) >=  (AnimationLength + .5) )
	{
		InAnimation = false;
		this->SetActorTickEnabled(false);
        UE_LOG(LogTemp,Warning, TEXT("BloxGrid::Tick::StartTileEndZ %f"), FloorArray[PlayerStartTileIndex]->GetActorLocation().Z);
		OnAnimationEndTrigger.Broadcast();
		return;
	}
	for (int Index : ValidFloors)
	{
		if (Index >= FloorArray.Num() || !FloorZOffset.Contains(Index)) continue;
		if (FloorArray[Index] == nullptr) continue;
		FVector FloorLocation = FloorArray[Index]->GetActorLocation();
		float Alpha = ((GetWorld()->TimeSeconds - AnimationStartTime) / AnimationLength);
		FloorLocation.Z = FMath::Lerp(*FloorZOffset.Find(Index) - StartAnimationBottomOffset, GetActorLocation().Z, Alpha);
		FloorLocation.Z = FMath::Clamp(FloorLocation.Z, GetActorLocation().Z - StartAnimationBottomOffset, GetActorLocation().Z);
		FloorArray[Index]->SetActorLocation(FloorLocation);
	}
}

bool ABloxGrid::SwitchTileType(int TileToSwitch, EBloxTileType TileType)
{
	UE_LOG(LogTemp, Warning, TEXT("SWITCHING TILE AT INDEX %d"), TileToSwitch);

	if (TileToSwitch < 0 || TileToSwitch > FloorArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempt to switch invalid tile index"));
		return false;
	}

	// Logic to switch the tile at given tile index
	if (!FloorArray[TileToSwitch])
	{
		UE_LOG(LogTemp, Warning, TEXT("NullTileSpot"));
		return false;
	}

	FVector Location = FloorArray[TileToSwitch]->GetActorLocation();
	FloorArray[TileToSwitch]->Destroy();
	FloorArray[TileToSwitch] = MakeTileInternal(Location, TileToSwitch, TileType, false);
	return FloorArray[TileToSwitch] ? true : false;
}

ABloxGridTile* ABloxGrid::MakeTile(EBloxTileType TileType)
{
	ABloxGridTile* NewTile;
	switch (TileType)
	{
	case EBloxTileType::END:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(EndTile));
		break;
	case EBloxTileType::BUTTON_SWITCH:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(ButtonSwitchTile));
		break;
	case EBloxTileType::CROSS_SWITCH:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(CrossSwitchTile));
		break;
	case EBloxTileType::BRIDGE_LEFT:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(BridgeTile));
		Cast<ABridgeTile>(NewTile)->SetDirection(TileType);
		break;
	case EBloxTileType::BRIDGE_RIGHT:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(BridgeTile));
		Cast<ABridgeTile>(NewTile)->SetDirection(TileType);
		break;
	case EBloxTileType::BRIDGE_UP:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(BridgeTile));
		Cast<ABridgeTile>(NewTile)->SetDirection(TileType);
		break;
	case EBloxTileType::BRIDGE_DOWN:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(BridgeTile));
		Cast<ABridgeTile>(NewTile)->SetDirection(TileType);
		break;
	case EBloxTileType::EMPTY:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(EmptyTile));
		break;
	case EBloxTileType::FALL:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(FallTile));
		break;
	case EBloxTileType::START:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(StartTile));
		break;
	case EBloxTileType::SPLIT:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(SplitTile));
		break;
	default:
		NewTile = Cast<ABloxGridTile>(GetWorld()->SpawnActor(Tile));
	}
	return NewTile;
}

EBloxTileType ABloxGrid::GetTileType(int TileIndex)
{
	if (TileIndex < 0 || TileIndex > FloorArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempt to switch invalid tile index"));
		return EBloxTileType::ENUM_END;
	}

	if (!FloorArray[TileIndex])
	{
		UE_LOG(LogTemp, Warning, TEXT("NullTileSpot"));
		return EBloxTileType::ENUM_END;
	}
	return FloorArray[TileIndex]->TileType;
}

FVector ABloxGrid::GetPlayerStart()
{
	if (FloorArray.IsEmpty() || PlayerStartTileIndex == -1 || PlayerStartTileIndex >= FloorArray.Num() || FloorArray[PlayerStartTileIndex] == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NullPointer To Start Tile"));
		return FVector::ZeroVector;
	}
	return FloorArray[PlayerStartTileIndex]->GetActorLocation();
}

FVector ABloxGrid::GetTileLocation(int TileIndexIn)
{
	if (TileIndexIn < 0 || TileIndexIn >= FloorArray.Num() || FloorArray[TileIndexIn] == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetTileLocation::Invalid Tile Index"));
		return FVector();
	}
	return FloorArray[TileIndexIn]->GetActorLocation();
}

ABloxGridTile* ABloxGrid::GetTileAtIndex(int TileIndex)
{
	if (TileIndex < 0 || TileIndex >= FloorArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetTileLocation::Invalid Tile Index"));
		return nullptr;
	}
	return FloorArray[TileIndex];
}

void ABloxGrid::HighlightTileAtIndex(int TileIndex, bool DoHighlight)
{
	if (TileIndex < 0 || TileIndex >= FloorArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("GetTileLocation::Invalid Tile Index"));
		return;
	}
	FloorArray[TileIndex]->HighlightTile(DoHighlight);
}

// Tile Factory
ABloxGridTile* ABloxGrid::MakeTileInternal(FVector TileLocation, int TileIndex, EBloxTileType TileType, bool bUseRelative)
{
	ABloxGridTile* NewTile = nullptr;
	ABlox* Player = nullptr;
	ABloxorzGameModeBase* GameModeBase = nullptr;


	switch (TileType)
	{
	case EBloxTileType::START:
		NewTile = MakeTile(TileType);
		PlayerStartTileIndex = TileIndex;
		break;

	case EBloxTileType::EMPTY:
		if (InEditorMode)
		{
			NewTile = MakeTile(TileType);
		}
		else
		{
			return nullptr;
		}
		break;

	case EBloxTileType::END:
		NewTile = MakeTile(TileType);
		Player = Cast<ABlox>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("ADDING DYNAMIC LISTENER"));
			NewTile->TileTrigger.AddDynamic(Player, &ABlox::StartEndAnimation);
			EndTileIndex = TileIndex;
		}
		break;

    case EBloxTileType::SPLIT:
		NewTile = MakeTile(TileType);
		GameModeBase =Cast<ABloxorzGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameModeBase)
		{
			UE_LOG(LogTemp, Warning, TEXT("Adding GameModeBase Listener"));
			NewTile->TileTrigger.AddDynamic(GameModeBase, &ABloxorzGameModeBase::SplitBloxPawn);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NULL GAME INSTANCE CAST"));
		}
        break;

	default:
		NewTile = MakeTile(TileType);
		break;
	}

	if (!NewTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid tile subclass"));
		return nullptr;
	}

	NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	if (bUseRelative)
	{
		NewTile->SetActorRelativeLocation(TileLocation, 0.f);
	}
	else
	{
		NewTile->SetActorLocation(TileLocation);
	}
	NewTile->SetActorScale3D(TileScale);
	FString TileName = "Floor" + FString::FromInt(TileIndex);
	//NewTile->SetActorLabel(TileName);
	NewTile->TileIndex = TileIndex;
	NewTile->TileType = TileType;
	return NewTile;
}

TArray<EBloxTileType> ABloxGrid::GetGridTileTypes()
{
	TArray<EBloxTileType> wTileTypes;
	for (ABloxGridTile* wGridTile : FloorArray)
    {
        if (!wGridTile)
        {
            wTileTypes.Add(EBloxTileType::EMPTY);
        }
        else
        {
            wTileTypes.Add(wGridTile->TileType);
        }
    }
	return wTileTypes;
}

void ABloxGrid::SerializeGrid(TArray<FString>& iLinkStrings, const FString& GridName)
{
	TArray<FString> SerializedLevel;
	FString Line = "";
	int ColumnSize = GridResolution;
	int CurrIndex = 0;
	Line.AppendInt(ColumnSize);
	SerializedLevel.Add(Line);
	Line = "";
	for (ABloxGridTile* GridTile : FloorArray)
	{
		if (CurrIndex % ColumnSize == 0)
		{
			SerializedLevel.Add(Line);
			Line = "";
		}

		if (!GridTile)
		{
			// Output * 
			Line.AppendChar('*');
		}

		else
		{
			// Output grid type to associated value
			// Create a static map from tile type to key
			char TypeChar = ' ';
			switch (GridTile->TileType)
			{
			case EBloxTileType::STANDARD:
				TypeChar = 's';
				break;
			case EBloxTileType::END:
				TypeChar = 'h';
				break;
			case EBloxTileType::START:
				TypeChar = 'b';
				break;
			case EBloxTileType::BUTTON_SWITCH:
				TypeChar = 'q';
				break;
			case EBloxTileType::CROSS_SWITCH:
				TypeChar = 'p';
				break;
			case EBloxTileType::EMPTY:
				TypeChar = '*';
				break;
			case EBloxTileType::BRIDGE_LEFT:
				TypeChar = 'i';
				break;
			case EBloxTileType::BRIDGE_RIGHT:
				TypeChar = 'p';
				break;
			case EBloxTileType::BRIDGE_UP:
				TypeChar = 'o';
				break;
			case EBloxTileType::BRIDGE_DOWN:
				TypeChar = 'l';
				break;
			case EBloxTileType::FALL:
				TypeChar = 'f';
				break;
			case EBloxTileType::SPLIT:
				TypeChar = 'k';
				break;
			default:
				TypeChar = '*';
				break;
			}
			Line.AppendChar(TypeChar);
		}
		++CurrIndex;
	}

	SerializedLevel.Add(UTF8_TO_TCHAR(BloxFileManager::CAMERA_START_INDICATOR));
	

	SerializedLevel.Add(UTF8_TO_TCHAR(BloxFileManager::LINK_START_INDICATOR));
	for (FString LinkLine: iLinkStrings)
	{
		SerializedLevel.Add(LinkLine);
	}

	UE_LOG(LogTemp, Warning, TEXT("SERIALIZE TO : "));
	UE_LOG(LogTemp, Warning, TEXT("Serialized Level Output: "));

	// Write data to file
	FString RelativePath = FPaths::ProjectContentDir().Append("/Maps/MapFiles/").Append(*GridName).Append(".bloxlevel");
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	bool Saved = FFileHelper::SaveStringArrayToFile(SerializedLevel, *RelativePath);
	if (!Saved)
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO SERIALIZE DATA"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("SUCCESSFULLY SERIALIZED DATA"));
	}
	FString Data; 
	FFileHelper::LoadFileToString(Data, *RelativePath, FFileHelper::EHashOptions::None);
	UE_LOG(LogTemp, Warning, TEXT("DATA READ FROM FILE %s"), *Data);
}

void ABloxGrid::RegisterBloxPawn(ABlox* BloxPawn)
{
	if (EndTileIndex < 0 || EndTileIndex >= FloorArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("INVALID END TILE INDEX"));
		return;
	}
	FloorArray[EndTileIndex]->TileTrigger.AddDynamic(BloxPawn, &ABlox::StartEndAnimation);
}

void ABloxGrid::ReadIntoLines(FString FileContent,TArray<FString>& FileLines, TArray<FString>& Links)
{
	FString Line;
	const FString LinkStartIndicator = "Links";
	bool ReadingLinks = false;
	for (int Index = 0; Index < FileContent.Len(); ++Index)
	{
		if (FileContent[Index] == '\n')
		{
			if (Line.Contains("Links", ESearchCase::IgnoreCase))
			{
				UE_LOG(LogTemp,Warning, TEXT("READING LINKS"));
				ReadingLinks = true;
				Line.Empty();
				continue;
			}

			if (ReadingLinks)
			{
				Links.Add(Line);
				UE_LOG(LogTemp, Warning, TEXT("LINK VALUE : %s"), *Line);
			}
			else if(!Line.IsEmpty())
			{
				FileLines.Add(Line);
				UE_LOG(LogTemp, Warning, TEXT("FILE LINE : %s"), *Line);
			}
			Line.Empty();
			continue;
		}
		else if (FChar::IsAlnum(FileContent[Index]) || FileContent[Index] == '-' || FileContent[Index] == '*')
        {
            Line.AppendChar(FileContent[Index]);
        }
	}
}

void ABloxGrid::Resize(int NewResolution)
{
    if(InAnimation || NewResolution == GridResolution) 
    {
        return;
    }
	InEditorMode = true;
    GridResolution = NewResolution;
    Initalize();

}

void ABloxGrid::Fill(EBloxTileType TileType)
{
	UE_LOG(LogTemp, Warning, TEXT("Fill Grid in BloxGrid"));
    for(int Index : ValidFloors)
    {
       SwitchTileType(Index, TileType); 
    }
}
