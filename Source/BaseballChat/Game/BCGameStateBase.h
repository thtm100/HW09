// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BCGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BASEBALLCHAT_API ABCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InName = FString(TEXT("None")));
	
};
