// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CommentEditOverlay.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

#define LOCTEXT_NAMESPACE "SCommentEditOverlay"

void SCommentEditOverlay::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	MaxCharacterCount = FMath::Max(0, InArgs._MaxCharacterCount);
	OnConfirmClicked = InArgs._OnConfirmClicked;
	OnCancelClicked = InArgs._OnCancelClicked;
	OnRemoveClicked = InArgs._OnRemoveClicked;
	InitialComment = InArgs._Content;

	TSharedPtr<SOverlay> Overlay;
	TSharedPtr<SHorizontalBox> ButtonSection;

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
					[
						SAssignNew(Overlay, SOverlay)
						.Visibility(EVisibility::SelfHitTestInvisible)
						+SOverlay::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						[
							CreateCommentWidget().ToSharedRef()
						]

						+SOverlay::Slot()
						.VAlign(VAlign_Bottom)
						.HAlign(HAlign_Fill)
						[
							SAssignNew(ButtonSection, SHorizontalBox)
							+SHorizontalBox::Slot()
							.VAlign(VAlign_Bottom)
							.HAlign(HAlign_Left)
							[
								CreateCancelButtonWidget().ToSharedRef()
							]
						]
					]
				]
			]
		]
	];

	if (!InitialComment.IsEmpty())
	{
		ButtonSection->AddSlot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		[
			CreateRemoveButtonWidget().ToSharedRef()
		];
	}

	ButtonSection->AddSlot()
	.VAlign(VAlign_Bottom)
	.HAlign(HAlign_Right)
	[
		CreateConfirmButtonWidget().ToSharedRef()
	];
}

TWeakPtr<SWindow> SCommentEditOverlay::GetParentWindow() const
{
	return ParentWindow;
}

TSharedPtr<SWidget> SCommentEditOverlay::CreateCommentWidget()
{
	return SNew(SBorder)
	.Padding(FMargin{24.0f, 24.0f, 24.0f, 62.0f})
	.Visibility(EVisibility::SelfHitTestInvisible)
	.BorderImage(FEditorStyle::GetBrush("Tutorials.Border"))
	.BorderBackgroundColor(FEditorStyle::Get().GetColor("Tutorials.Content.Color"))
	.ForegroundColor(FCoreStyle::Get().GetSlateColor("InvertedForeground"))
	[
		SNew(SBox)
		.Padding(FMargin{0.0f, 0.0f, 0.0f, 8.0f})
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
				.OnTextChanged_Lambda([this](const FText& Text)
				{
					FString Str = Text.ToString();
					if (Str.Len() > MaxCharacterCount)
					{
						Str.LeftInline(MaxCharacterCount);
						CommentTextBox->SetText(FText::FromString(Str));
					}
				})
			]
		]
	];
}

TSharedPtr<SWidget> SCommentEditOverlay::CreateCancelButtonWidget()
{
	return SNew(SButton)
	.OnClicked_Lambda([this]()
	{
		const bool bCommentChanged = CommentHasChanged();
		OnCancelClicked.ExecuteIfBound(bCommentChanged);
		return FReply::Handled();
	})
	.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationButtonWrapper"))
	.ContentPadding(0.0f)
	[
		SNew(SBox)
		.Padding(24.0f)
		[
			SNew(SBorder)
			.BorderImage(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationButton").Normal)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("Tutorials.Navigation.BackButton"))
					.ColorAndOpacity(FLinearColor::White)
				]

				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("CancelButton", "Cancel"))
					.TextStyle(FEditorStyle::Get(), "Tutorials.Content.NavigationText")
					.ColorAndOpacity(FLinearColor::White)
				]
			]
		]
	];
}

TSharedPtr<SWidget> SCommentEditOverlay::CreateRemoveButtonWidget()
{
	return SNew(SButton)
	.OnClicked_Lambda([this]()
	{
		OnRemoveClicked.ExecuteIfBound();
		return FReply::Handled();
	})
	.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationButtonWrapper"))
	.ContentPadding(0.0f)
	[
		SNew(SBox)
		.Padding(24.0f)
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
					.Text(LOCTEXT("RemoveButton", "Remove"))
					.TextStyle(FEditorStyle::Get(), "Tutorials.Content.NavigationText")
					.ColorAndOpacity(FLinearColor::White)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("MeshPaint.Remove"))
					.DesiredSizeOverride(FVector2D{32.0f, 32.0f})
					.ColorAndOpacity(FLinearColor::White)
				]
			]
		]
	];
}

bool SCommentEditOverlay::CommentHasChanged() const
{
	const FText CurrentText = CommentTextBox->GetText();
	return CurrentText.CompareTo(InitialComment) != 0;
}

TSharedPtr<SWidget> SCommentEditOverlay::CreateConfirmButtonWidget()
{
	return SNew(SButton)
	.OnClicked_Lambda([this]()
	{
		OnConfirmClicked.ExecuteIfBound(CommentTextBox->GetText());
		return FReply::Handled();
	})
	.IsEnabled_Lambda([this]()
	{
		return CommentHasChanged();
	})
	.ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Tutorials.Content.NavigationButtonWrapper"))
	.ContentPadding(0.0f)
	[
		SNew(SBox)
		.Padding(24.0f)
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
					.Text(LOCTEXT("AddCommentButton", "AddComment"))
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

#undef LOCTEXT_NAMESPACE