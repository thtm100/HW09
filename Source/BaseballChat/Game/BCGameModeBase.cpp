// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BCGameModeBase.h"

#include "BaseballChat.h"
#include "BCGameStateBase.h"
#include "Player/BCPlayerController.h"
#include "EngineUtils.h"
#include "Player/BCPlayerState.h"


ABCGameModeBase::ABCGameModeBase()
{
	bIsResetScheduled = false;
	RoundId = 0;
}

void ABCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABCPlayerController* PlayerController = Cast<ABCPlayerController>(NewPlayer);
	if (IsValid(PlayerController))
	{
		PlayerController->Notification =
			FText::FromString(TEXT("Connected to Server!"));
			
		AllPlayerControllers.Add(PlayerController);

		ABCPlayerState* PlayerState = PlayerController->GetPlayerState<ABCPlayerState>();
		if (IsValid(PlayerState))
		{
			PlayerState->PlayerName = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABCGameStateBase* GameStateBase = GetGameState<ABCGameStateBase>();
		if (IsValid(GameStateBase))
		{
			GameStateBase->MulticastRPCBroadcastLoginMessage(PlayerState->PlayerName);
		}
	}
}

FString ABCGameModeBase::GenerateRandomNum()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num)
	{
		return Num > 0;
	});

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 RandomIndex = FMath::RandRange(0, Numbers.Num() - 1);
		Result += FString::FromInt(Numbers[RandomIndex]);
		Numbers.RemoveAt(RandomIndex);
	}

	BC_LOG_NET(LogBCNet, Log, TEXT("[Round %d] RandomNum: %s"), RoundId, *Result);
	return Result;
}

bool ABCGameModeBase::IsGuessNum(const FString& InNum)
{
	bool bCanGuess = false;

	do
	{
		if (InNum.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNum)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}
			if (UniqueDigits.Contains(C))
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanGuess = true;
	} while (false);

	return bCanGuess;
}

FString ABCGameModeBase::JudgeResult(const FString& InRandomNum, const FString& InGuessNum)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InRandomNum[i] == InGuessNum[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessNum = FString::Printf(TEXT("%c"), InGuessNum[i]);
			if (InRandomNum.Contains(PlayerGuessNum))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS-%dB"), StrikeCount, BallCount);
}

void ABCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ResetGame();
}

void ABCGameModeBase::PrintChatMessage(ABCPlayerController* InChattingPlayerController, const FString& InChatMessage)
{
	ABCPlayerState* PS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();
	if (IsValid(PS))
	{
		if (PS->CurrentGuessCount > PS->MaxGuessCount)
		{
			InChattingPlayerController->ClientRPCPrintChatMessage(TEXT("You can't Guess ANYMORE."));
			
			return;
		}
	}
	
	int Index = InChatMessage.Len() - 3;
	FString GuessNum = InChatMessage.RightChop(Index);
	if (IsGuessNum(GuessNum))
	{
		FString JudgeResultString = JudgeResult(RandomNum, GuessNum);
		IncreaseGuessCount(InChattingPlayerController);
		
		for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It)
		{
			ABCPlayerController* PlayerController = *It;
			if (IsValid(PlayerController))
			{
				FString CombinedMessage = InChatMessage + TEXT(" -> ") + JudgeResultString;
				PlayerController->ClientRPCPrintChatMessage(CombinedMessage);

				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It)
		{
			ABCPlayerController* PlayerController = *It;
			if (IsValid(PlayerController))
			{
				FString CombinedMessage = TEXT("Please pick a right number: ") + InChatMessage + TEXT(" (X)");
				PlayerController->ClientRPCPrintChatMessage(CombinedMessage);
			}
		}
	}
}

void ABCGameModeBase::IncreaseGuessCount(ABCPlayerController* InChattingPlayerController)
{
	ABCPlayerState* PS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();
	if (IsValid(PS))
	{
		PS->CurrentGuessCount++;
	}
}

void ABCGameModeBase::ResetGame()
{
	GetWorldTimerManager().ClearTimer(ResetTimerHandle);
	BC_LOG_NET(LogBCNet, Log, TEXT(""));

	++RoundId;
	bIsResetScheduled = false;
	
	RandomNum = GenerateRandomNum();

	for (const auto& PlayerController : AllPlayerControllers)
	{
		ABCPlayerState* PS = PlayerController->GetPlayerState<ABCPlayerState>();
		if (IsValid(PS))
		{
			PS->CurrentGuessCount = 1;
		}
	}
}

void ABCGameModeBase::JudgeGame(ABCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		ABCPlayerState* PS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();
		for (const auto& PlayerController : AllPlayerControllers)
		{
			if (IsValid(PS))
			{
				FString CombinedMessage = PS->PlayerName + TEXT(" Wins!");
				PlayerController->Notification = FText::FromString(CombinedMessage);
			}
		}
		
		TryEndRound(InChattingPlayerController, true, false);
		return;
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& PlayerController : AllPlayerControllers)
		{
			ABCPlayerState* PS = PlayerController->GetPlayerState<ABCPlayerState>();
			if (IsValid(PS))
			{
				if (PS->CurrentGuessCount <= PS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (bIsDraw == true)
		{
			for (const auto& PlayerController : AllPlayerControllers)
			{
				PlayerController->Notification = FText::FromString(TEXT("Draw..."));
			}

			TryEndRound(InChattingPlayerController, false, true);
			return;
		}
	}
}

void ABCGameModeBase::ScheduleReset(float DelaySeconds)
{
	if (bIsResetScheduled == true)
	{
		return;
	}

	bIsResetScheduled = true;

	if (GetWorldTimerManager().IsTimerActive(ResetTimerHandle) == true)
	{
		GetWorldTimerManager().ClearTimer(ResetTimerHandle);
	}

	GetWorldTimerManager().SetTimer(
		ResetTimerHandle,
		this,
		&ThisClass::ResetGame,
		DelaySeconds,
		false
	);
}

void ABCGameModeBase::TryEndRound(ABCPlayerController* InChattingPlayerController, bool bIsWin, bool bIsDraw)
{
	if (bIsResetScheduled == true)
	{
		return;
	}

	if (bIsWin == true)
	{
		ScheduleReset();
		return;
	}

	if (bIsDraw == true)
	{
		ScheduleReset();
		return;
	}
}

