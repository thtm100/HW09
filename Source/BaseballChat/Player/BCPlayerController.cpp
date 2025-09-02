// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BCPlayerController.h"
#include "UI/BCChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BCGameModeBase.h"
#include "BCPlayerState.h"
#include "Net/UnrealNetwork.h"

ABCPlayerController::ABCPlayerController()
{
	bReplicates = true;
}

void ABCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputClass))
	{
		ChatInputInstance = CreateWidget<UBCChatInput>(this, ChatInputClass);
		if (IsValid(ChatInputInstance))
		{
			ChatInputInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationWidgetClass))
	{
		NotificationWidgetInstance = CreateWidget<UUserWidget>(this, NotificationWidgetClass);
		if (IsValid(NotificationWidgetInstance))
		{
			NotificationWidgetInstance->AddToViewport();
		}
	}
}

void ABCPlayerController::SetChatMessage(const FString& InChatMessage)
{
	if (IsLocalController())
	{
		ABCPlayerState* PS = GetPlayerState<ABCPlayerState>();
		if (IsValid(PS))
		{
			FString CombinedMessage = PS->GetPlayerInfo() + TEXT(": ") + InChatMessage;

			ServerRPCPrintChatMessage(CombinedMessage);
		}
	}
}

void ABCPlayerController::PrintChatMessage(const FString& InChatMessage)
{
	UKismetSystemLibrary::PrintString(this, InChatMessage,
		true, true, FLinearColor::Green, 5.f);	
}

void ABCPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Notification);
}

void ABCPlayerController::ServerRPCPrintChatMessage_Implementation(const FString& InChatMessage)
{
	ABCGameModeBase* GameModeBase =
		Cast<ABCGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (IsValid(GameModeBase))
	{
		GameModeBase->PrintChatMessage(this, InChatMessage);
	}
}

void ABCPlayerController::ClientRPCPrintChatMessage_Implementation(const FString& InChatMessage)
{
	PrintChatMessage(InChatMessage);
}
