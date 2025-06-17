// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Skate.h"
#include "SkateChar.h" // Classe de personagem

AGM_Skate::AGM_Skate()
{
    DefaultPawnClass = ASkateChar::StaticClass(); // Define o personagem padrão
}

