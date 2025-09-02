// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BCPlayerController.generated.h"

class UBCChatInput;
class UUserWidget;

/**
 * 
 */
UCLASS()
class BASEBALLCHAT_API ABCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABCPlayerController();
	
	virtual void BeginPlay() override;

	void SetChatMessage(const FString& InChatMessage);
	void PrintChatMessage(const FString& InChatMessage);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessage(const FString& InChatMessage);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessage(const FString& InChatMessage);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBCChatInput> ChatInputClass;

	UPROPERTY()
	TObjectPtr<UBCChatInput> ChatInputInstance;

	FString ChatMessage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationWidgetInstance;

public:	
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText Notification;
	
};
