// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BCPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BASEBALLCHAT_API ABCPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABCPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfo();

public:
	UPROPERTY(Replicated)
	FString PlayerName;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(EditAnywhere)
	int32 MaxGuessCount;
	
};
