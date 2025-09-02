// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "BCChatInput.generated.h"

class UEditableTextBox;

/**
 * 
 */
UCLASS()
class BASEBALLCHAT_API UBCChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputCommitted(const FText& InText, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInput;
	
};
