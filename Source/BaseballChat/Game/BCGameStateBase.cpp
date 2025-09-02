// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BCGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BCPlayerController.h"

void ABCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InName)
{
	if (HasAuthority() == false)
	{
		ABCPlayerController* PlayerController =
			Cast<ABCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		
		if (IsValid(PlayerController))
		{
			FString NotificationMessage = InName + TEXT(" has joined the game.");
			PlayerController->PrintChatMessage(NotificationMessage);
		}
	}
}
