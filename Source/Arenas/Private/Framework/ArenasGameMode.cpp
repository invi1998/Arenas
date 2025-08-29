// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ArenasGameMode.h"

APlayerController* AArenasGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	return Super::SpawnPlayerController(InRemoteRole, Options);
}
