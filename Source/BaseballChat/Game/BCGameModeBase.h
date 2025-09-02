// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BCGameModeBase.generated.h"

class ABCPlayerController;

/**
 * 
 */
UCLASS()
class BASEBALLCHAT_API ABCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABCGameModeBase();
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateRandomNum();
	bool IsGuessNum(const FString& InNum);
	FString JudgeResult(const FString& InRandomNum, const FString& InGuessNum);

	virtual void BeginPlay() override;

	void PrintChatMessage(ABCPlayerController* InChattingPlayerController, const FString& InChatMessage);

	void IncreaseGuessCount(ABCPlayerController* InChattingPlayerController);

	void ResetGame();
	void JudgeGame(ABCPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	void ScheduleReset(float DelaySeconds = 1.f);

	void TryEndRound(ABCPlayerController* InChattingPlayerController, bool bIsWin, bool bIsDraw);
	

protected:
	FString RandomNum;

	TArray<TObjectPtr<ABCPlayerController>> AllPlayerControllers;

	UPROPERTY(Transient)
	uint8 bIsResetScheduled : 1;

	UPROPERTY(Transient)
	int32 RoundId;

	FTimerHandle ResetTimerHandle;
	
};
