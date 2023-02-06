// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/SCompoundWidget.h"

class SMultiLineEditableTextBox;
class SCanvas;
class SWindow;

DECLARE_DELEGATE_OneParam(FOnConfirmAddComment, FText)
DECLARE_DELEGATE_OneParam(FOnCancelAddComment, bool)

class SCommentEditOverlay : public SCompoundWidget
{
public:
	static constexpr int32 DefaultMaxCharacterCount = 1024;

	SLATE_BEGIN_ARGS(SCommentEditOverlay)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
		_MaxCharacterCount = DefaultMaxCharacterCount;
	}
		SLATE_ARGUMENT(TWeakPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(FText, Content)
		SLATE_ARGUMENT(int32, MaxCharacterCount)
		SLATE_EVENT(FOnConfirmAddComment, OnConfirmClicked)
		SLATE_EVENT(FOnCancelAddComment, OnCancelClicked)
		SLATE_EVENT(FSimpleDelegate, OnRemoveClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TWeakPtr<SWindow> GetParentWindow() const;

private:
	TSharedPtr<SWidget> CreateCommentWidget();
	TSharedPtr<SWidget> CreateConfirmButtonWidget();
	TSharedPtr<SWidget> CreateCancelButtonWidget();
	TSharedPtr<SWidget> CreateRemoveButtonWidget();

	bool CommentHasChanged() const;

	TSharedPtr<SCanvas> OverlayCanvas;
	TWeakPtr<SWindow> ParentWindow;

	FOnConfirmAddComment OnConfirmClicked;
	FOnCancelAddComment OnCancelClicked;
	FSimpleDelegate OnRemoveClicked;

	TSharedPtr<SMultiLineEditableTextBox> CommentTextBox;
	int32 MaxCharacterCount;
	FText InitialComment;
};