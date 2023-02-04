// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CommentEditOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

void SCommentEditOverlay::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	OnConfirmClicked = InArgs._OnConfirmClicked;
	OnCancelClicked = InArgs._OnCancelClicked;

	TSharedPtr<SOverlay> Overlay;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SBox)
					.Padding(8.0f)
					[
						SAssignNew(Overlay, SOverlay)
						.Visibility(EVisibility::SelfHitTestInvisible)
						+SOverlay::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						[
							CreateCommentWidget(InArgs._Content).ToSharedRef()
						]

						+SOverlay::Slot()
						.VAlign(VAlign_Bottom)
						.HAlign(HAlign_Left)
						.Padding(12.0f)
						[
							CreateCancelButtonWidget().ToSharedRef()
						]

						+SOverlay::Slot()
						.VAlign(VAlign_Bottom)
						.HAlign(HAlign_Right)
						.Padding(12.0f)
						[
							CreateConfirmButtonWidget().ToSharedRef()
						]
					]
				]
			]
		]
	];
}

TSharedPtr<SWidget> SCommentEditOverlay::CreateCommentWidget(const FText& InitialComment)
{
	return SNew(SBorder)
	.Padding(FMargin{24.0f, 24.0f, 24.0f, 62.0f})
	.Visibility(EVisibility::SelfHitTestInvisible)
	.BorderImage(FEditorStyle::GetBrush("Tutorials.Border"))
	.BorderBackgroundColor(FEditorStyle::Get().GetColor("Tutorials.Content.Color"))
	.ForegroundColor(FCoreStyle::Get().GetSlateColor("InvertedForeground"))
	[
		SNew(SBox)
		.Padding(8.0f)
		.MinDesiredWidth(600.0f)
		.MaxDesiredWidth(1000.0f)
		.MinDesiredHeight(400.f)
		.MaxDesiredHeight(800.f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(CommentTextBox, SMultiLineEditableTextBox)
				.Visibility(EVisibility::SelfHitTestInvisible)
				.AutoWrapText(true)
				.Text(InitialComment)
				.TextStyle(FEditorStyle::Get(), "Tutorials.Content")
				// SNew(SHorizontalBox)
				// +SHorizontalBox::Slot()
				// .HAlign(HAlign_Fill)
				// .VAlign(VAlign_Fill)
				// [

					// SNew(STextBlock)
					// .Visibility(EVisibility::SelfHitTestInvisible)
					// .AutoWrapText(true)
					// .Text(Content)
					// .TextStyle(FEditorStyle::Get(), "Tutorials.Content")
					// .HighlightColor(FEditorStyle::Get().GetColor("Tutorials.Browser.HighlightTextColor"))
				// ]
			]
		]
	];
}

TSharedPtr<SWidget> SCommentEditOverlay::CreateCancelButtonWidget()
{
	return SNew(SButton)
	.OnClicked_Lambda([this]()
	{
		OnCancelClicked.Execute();
		return FReply::Handled();
	})
	.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationButtonWrapper"))
	.ContentPadding(0.0f)
	[
		SNew(SBox)
		.Padding(8.0f)
		[
			SNew(SBorder)
			.BorderImage(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationBackButton").Normal)
			[
				 SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("Tutorials.Navigation.BackButton"))
					.ColorAndOpacity(FLinearColor::White)
				]
			]
		]
	];
}

TSharedPtr<SWidget> SCommentEditOverlay::CreateConfirmButtonWidget()
{
	return SNew(SButton)
	.OnClicked_Lambda([this]()
	{
		OnConfirmClicked.Execute(CommentTextBox->GetText());
		return FReply::Handled();
	})
	.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationButtonWrapper"))
	.ContentPadding(0.0f)
	[
		SNew(SBox)
		.Padding(8.0f)
		[
			SNew(SBorder)
			.BorderImage(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationButton").Normal)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(4.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Add Comment")))
					.TextStyle(FEditorStyle::Get(), "Tutorials.Content.NavigationText")
					.ColorAndOpacity(FLinearColor::White)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("Tutorials.Navigation.NextButton"))
					.ColorAndOpacity(FLinearColor::White)
				]
			]
		]
	];
}