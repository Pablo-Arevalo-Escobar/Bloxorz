// Fill out your copyright notice in the Description page of Project Settings.


#include "EndTile.h"
#include "Blox.h"
AEndTile::AEndTile()
{
    //BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    //BoxComponent->SetupAttachment(RootComponent);
    //BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEndTile::OnBoxBeginOverlap);
}


void AEndTile::Trigger(ABlox* BloxActor)
{
    if (BloxActor->GetBloxState() != BloxState::STANDING || GetWorld()->TimeSeconds - PreviosTimePressed <= DownTime)
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("ENDTRIGGER"));
    TileTrigger.Broadcast(this);
    PreviosTimePressed = GetWorld()->TimeSeconds;
    OnEndAnimation();
}

void AEndTile::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //if (GetWorld()->TimeSeconds - PreviosTimePressed <= DownTime)
    //{
    //    return;
    //}
    //UE_LOG(LogTemp, Warning, TEXT("ENDTRIGGER"));
    //TileTrigger.Broadcast(TileType);
    //PreviosTimePressed = GetWorld()->TimeSeconds;
    //OnEndAnimation();
}
