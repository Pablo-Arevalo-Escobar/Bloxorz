// Fill out your copyright notice in the Description page of Project Settings.


#include "BridgeTile.h"

ABridgeTile::ABridgeTile()
{
    BridgeTile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BridgeTile"));
    PivotLeft = CreateDefaultSubobject<USceneComponent>(TEXT("LeftPivot"));
    PivotRight = CreateDefaultSubobject<USceneComponent>(TEXT("RightPivot"));
    PivotUp = CreateDefaultSubobject<USceneComponent>(TEXT("UpPivot"));
    PivotDown = CreateDefaultSubobject<USceneComponent>(TEXT("DownPivot"));

    BridgeTile->SetupAttachment(RootComponent);
    PivotLeft->SetupAttachment(RootComponent);
    PivotRight->SetupAttachment(RootComponent);
    PivotUp->SetupAttachment(RootComponent);
    PivotDown->SetupAttachment(RootComponent);
}

void ABridgeTile::SetDirection(EBloxTileType Type)
{
    this->TileType = Type;
    switch (TileType)
    {
    case EBloxTileType::BRIDGE_LEFT:
        BridgeTile->AttachToComponent(PivotLeft, FAttachmentTransformRules::KeepWorldTransform);
        break;
    case EBloxTileType::BRIDGE_RIGHT:
        BridgeTile->AttachToComponent(PivotRight, FAttachmentTransformRules::KeepWorldTransform);
        break;
    case EBloxTileType::BRIDGE_UP:
        BridgeTile->AttachToComponent(PivotUp, FAttachmentTransformRules::KeepWorldTransform);
        break;
    case EBloxTileType::BRIDGE_DOWN:
        BridgeTile->AttachToComponent(PivotDown, FAttachmentTransformRules::KeepWorldTransform);
        break;
    }
}

void ABridgeTile::ToggleBridge()
{
    switch (TileType)
    {
    case EBloxTileType::BRIDGE_LEFT:
        PivotLeft->AddRelativeRotation(FRotator(180, 0, 0));
        break;
    case EBloxTileType::BRIDGE_RIGHT:
        PivotRight->AddRelativeRotation(FRotator(180, 0, 0));
        break;
    case EBloxTileType::BRIDGE_UP:
        PivotUp->AddRelativeRotation(FRotator(0, 0, 180));
        break;
    case EBloxTileType::BRIDGE_DOWN:
        PivotDown->AddRelativeRotation(FRotator(0, 0, 180));
        break;
    }
}

void ABridgeTile::LinkReceived(ABloxGridTile* GridTile)
{
    UE_LOG(LogTemp, Warning, TEXT("Bridge Tile Link Received"));
    ToggleBridge();
}

void ABridgeTile::RotateAroundPoint(UPrimitiveComponent* Component, FVector RotationPoint, FRotator Rotation)
{
    // Get the current actor location and rotation
    FVector CurrentLocation = BridgeTile->GetComponentLocation();
    FRotator CurrentRotation = BridgeTile->GetComponentRotation();

    // Rotate the actor around the specified point
    FVector RotatedLocation = FQuat(Rotation) * (CurrentLocation - RotationPoint) + RotationPoint;

    // Set the new location and rotation for the component
    Component->SetWorldLocationAndRotation(RotatedLocation, CurrentRotation + Rotation);
}
