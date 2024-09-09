#pragma once
#include <CoreMinimal.h>
#include <string>

#include "Bloxorz/BloxGridTile.h"

DECLARE_LOG_CATEGORY_EXTERN(BloxFileManagerLog, Log, All);

BLOXFILEMANAGER_API struct FBloxGridData
{
    int mGridResolution;
    TArray<EBloxTileType> mGridTiles;
    FTransform mCameraTransform;
    TMap<int, TSet<int>> mLinkMap;
    FString mGridName;
};

/*
* .bloxlevel file definition
* GRID RESOLUTION (LINE 0)
* @ TILE SPECIFICATION
* @ CAMERA POSITION AND TRANSFORM
* @ LINKS
*/

namespace BloxFileManager {
const char LINK_START_INDICATOR = '^';
const char CAMERA_START_INDICATOR = '@';

BLOXFILEMANAGER_API  EBloxTileType ParseTileType(const char& c);

BLOXFILEMANAGER_API  void LoadLink(FString& wLine, FBloxGridData& iGridData);

BLOXFILEMANAGER_API  FBloxGridData Parse(FString iFilePath);

BLOXFILEMANAGER_API  void Serialize(FBloxGridData& iGridData);

}