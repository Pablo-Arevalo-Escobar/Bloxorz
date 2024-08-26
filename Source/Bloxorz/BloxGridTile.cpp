// Fill out your copyright notice in the Description page of Project Settings.
#include "BloxGridTile.h"
#include "Blox.h"

// Sets default values
ABloxGridTile::ABloxGridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;
	//TileMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABloxGridTile::BeginPlay()
{
	Super::BeginPlay();
}

void ABloxGridTile::Trigger(ABlox* BloxActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Standard Trigger"));
}

void ABloxGridTile::LinkReceived(ABloxGridTile* TriggerTile)
{
	UE_LOG(LogTemp, Warning, TEXT("LINK RECEIVED"));
}

// Called every frame
void ABloxGridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("Tile actor tick enabled"));
}

void ABloxGridTile::HighlightTile(bool bToHighlight)
{
	UE_LOG(LogTemp, Warning, TEXT("Highlighting Tile"));
	if (bToHighlight)
	{
		TileMesh->SetOverlayMaterial(OverlayMaterial);
	}
	else
	{
		TileMesh->SetOverlayMaterial(false);
	}
}

EBloxTileType ABloxGridTile::GetTileType()
{
	return TileType;
}