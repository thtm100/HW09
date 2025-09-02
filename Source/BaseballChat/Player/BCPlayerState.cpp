// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BCPlayerState.h"
#include "Net/UnrealNetwork.h"

ABCPlayerState::ABCPlayerState() : PlayerName(TEXT("None")), CurrentGuessCount(1), MaxGuessCount(3)
{
	bReplicates = true;
}

void ABCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerName);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
}

FString ABCPlayerState::GetPlayerInfo()
{
	FString PlayerInfoString =
		PlayerName
		+ TEXT("(")
		+ FString::FromInt(CurrentGuessCount)
		+ TEXT("/")
		+ FString::FromInt(MaxGuessCount)
		+ TEXT(")");

	return PlayerInfoString;
}
