// Fill out your copyright notice in the Description page of Project Settings.


#include "FallTile.h"
#include "Blox.h"

AFallTile::AFallTile()
{
    //BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    //BoxComponent->SetupAttachment(RootComponent);
    //BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFallTile::OnBoxBeginOverlap);
}

void AFallTile::Trigger(ABlox* BloxActor)
{
    if (!BloxActor || BloxActor->GetBloxState() != BloxState::STANDING)
        return;
    UE_LOG(LogTemp, Warning, TEXT("Fall Trigger"));
    TileMesh->SetSimulatePhysics(true);
    TileMesh->AddImpulse(FVector::DownVector);
    BloxActor->Fall();
}

void AFallTile::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //if (OtherActor && OtherActor->IsA(ABlox::StaticClass()))
    //{
    //    ABlox* Blox = Cast<ABlox>(OtherActor);
    //    if (!Blox || Blox->GetBloxState() != BloxState::STANDING)
    //        return;

    //    TileMesh->SetSimulatePhysics(true);
    //    TileMesh->AddImpulse(FVector::DownVector);
    //    Blox->Fall();
    //}
}
