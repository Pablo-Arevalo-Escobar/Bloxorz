// Fill out your copyright notice in the Description page of Project Settings.


#include "ModuleActorBase.h"
// Sets default values
AModuleActorBase::AModuleActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AModuleActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AModuleActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Blue, TEXT("Hello, world!"));
}

