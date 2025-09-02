// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BCChatInput.h"
#include "Components/EditableTextBox.h"
#include "Player/BCPlayerController.h"

void UBCChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputCommitted) == false)
	{
		ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputCommitted);
	}
}

void UBCChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputCommitted) == true)
	{
		ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputCommitted);
	}
}

void UBCChatInput::OnChatInputCommitted(const FText& InText, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ABCPlayerController* PlayerController = Cast<ABCPlayerController>(GetOwningPlayer());
		if (IsValid(PlayerController))
		{
			PlayerController->SetChatMessage(InText.ToString());

			ChatInput->SetText(FText());
		}
	}
}
