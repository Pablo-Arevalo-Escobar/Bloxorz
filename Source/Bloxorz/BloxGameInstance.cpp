// Fill out your copyright notice in the Description page of Project Settings.


#include "BloxGameInstance.h"

void UBloxGameInstance::Init()
{
    Super::Init();
    NumberOfMoves = 0;
    MoveDelta = 0;
}

void UBloxGameInstance::ApplyMoveDelta()
{
    NumberOfMoves += MoveDelta;
    MoveDelta = 0;
}

void UBloxGameInstance::IncrementMoveDelta()
{
    ++MoveDelta;
}

void UBloxGameInstance::ResetMoveDelta()
{
    MoveDelta = 0;
}

int32 UBloxGameInstance::GetCurrentMovesValue()
{
    return MoveDelta + NumberOfMoves;
}
