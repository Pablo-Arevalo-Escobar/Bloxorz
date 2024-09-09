// Fill out your copyright notice in the Description page of Project Settings.
#include "BloxFileManager.h"

DEFINE_LOG_CATEGORY(BloxFileManagerLog);

namespace BloxFileManager {

static const TMap<char, EBloxTileType> ParseTileMap{
	{'s',EBloxTileType::STANDARD},
	{'b',EBloxTileType::START},
	{'h',EBloxTileType::END},
	{'q',EBloxTileType::BUTTON_SWITCH},
	{'e',EBloxTileType::CROSS_SWITCH},
	{'f',EBloxTileType::FALL},
	{'*',EBloxTileType::EMPTY},
	{'i',EBloxTileType::BRIDGE_LEFT},
	{'p',EBloxTileType::BRIDGE_RIGHT},
	{'o',EBloxTileType::BRIDGE_UP},
	{'l',EBloxTileType::BRIDGE_DOWN},
	{'k',EBloxTileType::SPLIT},
};

static enum EParsingState {
	EReadHeaderInfo, EReadTileInfo, EReadCameraInfo, EReadLinkInfo, EEnumEnd
};

EBloxTileType ParseTileType(const char& c)
{
	if (ParseTileMap.Contains(c))
	{
		return *ParseTileMap.Find(c);
	}
	else
	{
		UE_LOG(BloxFileManagerLog, Warning, TEXT("Invalid character parsed when reading level file %c"), c);
		return EBloxTileType::ENUM_END;
	}
}

void LoadLink(FString& wLine, FBloxGridData& iGridData)
{
	if (wLine.IsEmpty()) return;

	// PARSE THE LOAD LINK STRING INTO RECEIVER AND TRIGGER
	FString wTriggerTileIndexString;
	FString wReceiveTileIndexString;
	const char LINK_SEPERATOR = '-';
	bool wSeperatorEncountered = false;
	int wLineIndex = 0;
	char wCurrChar = '!';
	while (wCurrChar != '\n' && wLineIndex < wLine.Len())
	{
		wCurrChar = wLine[wLineIndex];
		if (wCurrChar == LINK_SEPERATOR)
		{
			wSeperatorEncountered = true;
			++wLineIndex;
			wCurrChar = wLine[wLineIndex];
			continue;
		}
		if (!wSeperatorEncountered)
		{
			wTriggerTileIndexString.AppendChar(wCurrChar);
		}
		else
		{
			wReceiveTileIndexString.AppendChar(wCurrChar);
		}
		++wLineIndex;
	}
	
	if (wTriggerTileIndexString.IsEmpty() || wReceiveTileIndexString.IsEmpty())
	{
		return;
	}

	int wTriggerTileIndex = FCString::Atoi(*wTriggerTileIndexString);
	int wReceiveTileIndex = FCString::Atoi(*wReceiveTileIndexString);

	int32 wNumberOfTiles = iGridData.mGridResolution * iGridData.mGridResolution;
	if (wTriggerTileIndex < 0 || wTriggerTileIndex > wNumberOfTiles || wReceiveTileIndex < 0 || wReceiveTileIndex > wNumberOfTiles)
	{
		UE_LOG(BloxFileManagerLog, Warning, TEXT("Invalid index used for link tile"));
		return;
	}

	// LOAD LINK INTO GRID DATA
	if (!iGridData.mLinkMap.Contains(wTriggerTileIndex))
	{
		iGridData.mLinkMap.Add(wTriggerTileIndex, TSet<int>());
	}
	iGridData.mLinkMap.Find(wTriggerTileIndex)->Add(wReceiveTileIndex);
}

FBloxGridData Parse(FString iFilePath)
{
	FString RelativePath = FPaths::ProjectContentDir().Append("/Maps/MapFiles/").Append(iFilePath).Append(".bloxlevel");
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (!FileManager.FileExists(*RelativePath))
	{
		UE_LOG(BloxFileManagerLog, Warning, TEXT("Level load failed. Grid file does not exist. File name %s"), *RelativePath);
		return FBloxGridData();
	}

	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *RelativePath, FFileHelper::EHashOptions::None))
	{
		UE_LOG(BloxFileManagerLog, Warning, TEXT("Level file exists but cannot be loaded to string"));
		return FBloxGridData();
	}

	FBloxGridData wGridData = { 0, TArray<EBloxTileType>(), FTransform::Identity, TMap<int,TSet<int>>() };

	// Read data from file into the GridData struct
	EParsingState wParsingState = EReadHeaderInfo;
	FString wLine;
	for (int wIndex = 0; wIndex < FileContent.Len(); ++wIndex)
	{
		if (FileContent[wIndex] == '\r' || FileContent[wIndex] == '\t' || FileContent[wIndex] == ' ') continue;
		switch (wParsingState)
		{
		case EReadHeaderInfo:
			if (FileContent[wIndex] == '\n' && !wLine.IsEmpty())
			{
				wGridData.mGridResolution = wLine.IsNumeric() ? std::stoi(*wLine) : -1;
				wParsingState = EReadTileInfo;
				wLine.Empty();
			}
			wLine += FileContent[wIndex];
			break;

		case EReadTileInfo:
			if (FileContent[wIndex] == CAMERA_START_INDICATOR)
			{
				wParsingState = EReadCameraInfo;
			}
			else if (FChar::IsAlnum(FileContent[wIndex]) || FileContent[wIndex] == '*')
			{
				EBloxTileType TileType = BloxFileManager::ParseTileType(FileContent[wIndex]);
				wGridData.mGridTiles.Add(TileType);
			}
			break;

		case EReadCameraInfo:
			if (FileContent[wIndex] == LINK_START_INDICATOR)
			{
				wParsingState = EReadLinkInfo;
			}
			break;

		case EReadLinkInfo:
			if (FileContent[wIndex] == '\n')
			{
				BloxFileManager::LoadLink(wLine, wGridData);
				wLine.Empty();
			}
			else
			{
				wLine.AppendChar(FileContent[wIndex]);
			}
			break;

		default:
			break;
		}
	}

	return wGridData;
}

void Serialize(FBloxGridData& iGridData)
{
	// WRITE HEADER
	TArray<FString> wSerializedLevel;
	wSerializedLevel.Add(FString::FromInt(iGridData.mGridResolution));
	FString wLine = "";
	int wCurrIndex = 0;
	
	// WRITE TILES
	for (EBloxTileType& wTileType : iGridData.mGridTiles)
	{
		if (wCurrIndex % iGridData.mGridResolution == 0)
		{
			wSerializedLevel.Add(wLine);
			wLine = "";
		}

		char wTypeChar = ' ';
		switch (wTileType)
		{
		case EBloxTileType::STANDARD:
			wTypeChar = 's';
			break;
		case EBloxTileType::END:
			wTypeChar = 'h';
			break;
		case EBloxTileType::START:
			wTypeChar = 'b';
			break;
		case EBloxTileType::BUTTON_SWITCH:
			wTypeChar = 'q';
			break;
		case EBloxTileType::CROSS_SWITCH:
			wTypeChar = 'p';
			break;
		case EBloxTileType::EMPTY:
			wTypeChar = '*';
			break;
		case EBloxTileType::BRIDGE_LEFT:
			wTypeChar = 'i';
			break;
		case EBloxTileType::BRIDGE_RIGHT:
			wTypeChar = 'p';
			break;
		case EBloxTileType::BRIDGE_UP:
			wTypeChar = 'o';
			break;
		case EBloxTileType::BRIDGE_DOWN:
			wTypeChar = 'l';
			break;
		case EBloxTileType::FALL:
			wTypeChar = 'f';
			break;
		case EBloxTileType::SPLIT:
			wTypeChar = 'k';
			break;
		default:
			wTypeChar = '*';
			break;
		}
		wLine.AppendChar(wTypeChar);
		++wCurrIndex;
	}
	
	// WRITE CAMERA
	wLine = "";
	wLine.AppendChar(BloxFileManager::CAMERA_START_INDICATOR);
	wSerializedLevel.Add(wLine);
	
	FVector wCameraLocation = iGridData.mCameraTransform.GetLocation();
	FRotator wCameraRotation = iGridData.mCameraTransform.GetRotation().Rotator();
	wSerializedLevel.Add(FString::Printf(TEXT("%f,%f,%f,%f,%f,%f"), wCameraLocation.X, wCameraLocation.Y, wCameraLocation.Z, wCameraRotation.Pitch, wCameraRotation.Yaw, wCameraRotation.Roll));


	// WRITE LINKS
	wLine = "";
	wLine.AppendChar(BloxFileManager::LINK_START_INDICATOR);
	wSerializedLevel.Add(wLine);
	if (!iGridData.mLinkMap.IsEmpty())
	{
		wSerializedLevel.Add(TEXT("Links"));
		for (auto& wKeyValPair : iGridData.mLinkMap)
		{
			int wLinkCaller = wKeyValPair.Key;
			for (int wLinkReceiver : wKeyValPair.Value)
			{
				wSerializedLevel.Add(FString::Printf(TEXT("%d-%d"), wLinkCaller, wLinkReceiver));
			}
		}
	}

	// WRITE TO FILE
	FString RelativePath = FPaths::ProjectContentDir().Append("/Maps/MapFiles/").Append(*iGridData.mGridName).Append(".bloxlevel");
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	bool Saved = FFileHelper::SaveStringArrayToFile(wSerializedLevel, *RelativePath);
	if (!Saved)
	{
		UE_LOG(BloxFileManagerLog, Error, TEXT("FAILED TO SERIALIZE DATA"));
	}
	else {
		UE_LOG(BloxFileManagerLog, Log, TEXT("SUCCESSFULLY SERIALIZED DATA"));
	}

#if WITH_EDITOR
	FString Data;
	FFileHelper::LoadFileToString(Data, *RelativePath, FFileHelper::EHashOptions::None);
	UE_LOG(LogTemp, Warning, TEXT("DATA READ FROM FILE %s"), *Data);
#endif
}
} // namespace