// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonTile.h"
#include <Kismet/GameplayStatics.h>

AButtonTile::AButtonTile()
{
    ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
    //BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    
    ButtonMesh->SetupAttachment(RootComponent);
    // BoxComponent->SetupAttachment(ButtonMesh);
    //BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AButtonTile::OnBoxBeginOverlap);
}

void AButtonTile::BeginPlay()
{
    Super::BeginPlay();

    // Snap to grid start if it exists
    TArray<AActor*> FoundActors;
    Player =  Cast<ABlox>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    if (!Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("Unable to find player from tile"));
    }
}

void AButtonTile::Trigger(ABlox* BloxActor)
{
    if (GetWorld()->TimeSeconds - PreviosTimePressed <= DownTime)
    {
        return;
    }
    // TODO: Add functionality to wait one move after the box overlaps the button tile
    UE_LOG(LogTemp, Warning, TEXT("BoxBeginOverlap"));
    switch (TileType)
    {
    case EBloxTileType::BUTTON_SWITCH:
        TileTrigger.Broadcast(this);
        break;
    case EBloxTileType::CROSS_SWITCH:
        if (!Player)
            return;
        if (Player->GetBloxState() == BloxState::STANDING)
            TileTrigger.Broadcast(this);
        break;
    }
    PreviosTimePressed = GetWorld()->TimeSeconds;
}
