// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateGameMode.h"
#include "SkateboardPawn.h"

ASkateGameMode::ASkateGameMode()
{
    // Define SkateboardPawn como a classe padr�o de Pawn
    DefaultPawnClass = ASkateboardPawn::StaticClass();
}

