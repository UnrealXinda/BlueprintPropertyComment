// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/SCompoundWidget.h"

class SMultiLineEditableTextBox;
class SCanvas;
class SWindow;

DECLARE_DELEGATE_OneParam(FOnConfirmAddComment, FText)

class SCommentEditOverlay : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCommentEditOverlay)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}
		SLATE_ARGUMENT(TWeakPtr<SWindow>, ParentWindow)
		SLATE_ARGUMENT(FText, Content)
		SLATE_EVENT(FOnConfirmAddComment, OnConfirmClicked)
		SLATE_EVENT(FSimpleDelegate, OnCancelClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SWidget> CreateCommentWidget(const FText& InitialComment);
	TSharedPtr<SWidget> CreateConfirmButtonWidget();
	TSharedPtr<SWidget> CreateCancelButtonWidget();

	TSharedPtr<SCanvas> OverlayCanvas;
	TWeakPtr<SWindow> ParentWindow;

	FOnConfirmAddComment OnConfirmClicked;
	FSimpleDelegate OnCancelClicked;
	// TODO: add remove button
	//FSimpleDelegate OnRemoveClicked;

	TSharedPtr<SMultiLineEditableTextBox> CommentTextBox;
};