// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintPropertyCommentEditor.h"
#include "PropertyCommentExtension.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UI/CommentEditOverlay.h"

#define LOCTEXT_NAMESPACE "FBlueprintPropertyCommentEditorModule"

namespace
{
	FString GetPropertyPathRecursive(const TSharedPtr<IPropertyHandle>& InHandle)
	{
		if (!InHandle.IsValid() || !InHandle->IsValidHandle())
		{
			return FString{};
		}

		TSharedPtr<IPropertyHandle> Handle = InHandle;
		TArray<FString, TInlineAllocator<16>> Paths;

		while (Handle && Handle->IsValidHandle())
		{
			if (FString SubPath = Handle->GeneratePathToProperty(); !SubPath.IsEmpty())
			{
				Paths.Add(MoveTemp(SubPath));
			}
			Handle = Handle->GetParentHandle();
		}

		Algo::Reverse(Paths);
		const FString Path = FString::Join(Paths, TEXT("->"));
		return Path;
	}

	UBlueprint* GetBlueprintFromPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		UBlueprint* Blueprint = nullptr;

		if (PropertyHandle && PropertyHandle->GetNumOuterObjects() == 1)
		{
			TArray<UObject*> OuterObjects;
			PropertyHandle->GetOuterObjects(OuterObjects);
			const UObject* Outer = OuterObjects[0];
			if (Outer->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
			{
				for (; IsValid(Outer); Outer = Outer->GetOuter())
				{
					const UClass* OuterClass = Outer->GetClass();
					if (Outer->IsA<UBlueprintGeneratedClass>())
					{
						Blueprint = UBlueprint::GetBlueprintFromClass(Cast<UBlueprintGeneratedClass>(Outer));
						break;
					}

					Blueprint = UBlueprint::GetBlueprintFromClass(OuterClass);
					if (IsValid(Blueprint))
					{
						break;
					}
				}
			}
		}

		return Blueprint;
	}

	bool BlueprintHasComment(const UBlueprint* Blueprint, const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		check(IsValid(Blueprint))

		const UPropertyCommentExtension* Extension = UPropertyCommentExtension::GetPropertyCommentExtension(Blueprint);
		if (!IsValid(Extension))
		{
			return false;
		}

		const FName PropertyKey = FName{GetPropertyPathRecursive(PropertyHandle)};
		return Extension->HasComment(PropertyKey);
	}
}

void FBlueprintPropertyCommentEditorModule::StartupModule()
{
	RegisterDetailRowExtension();
	RegisterAssetRegistryActions();
}

void FBlueprintPropertyCommentEditorModule::ShutdownModule()
{
	DeregisterDetailRowExtension();
	DeregisterAssetRegistryActions();
}

void FBlueprintPropertyCommentEditorModule::RegisterDetailRowExtension()
{
	FPropertyEditorModule& Module = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FOnGenerateGlobalRowExtension& RowExtensionDelegate = Module.GetGlobalRowExtensionDelegate();
	RowExtensionDelegate.AddRaw(this, &FBlueprintPropertyCommentEditorModule::HandleCreatePropertyRowExtension);
}

void FBlueprintPropertyCommentEditorModule::DeregisterDetailRowExtension()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& Module = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		Module.GetGlobalRowExtensionDelegate().RemoveAll(this);
	}
}

void FBlueprintPropertyCommentEditorModule::HandleCreatePropertyRowExtension(const FOnGenerateGlobalRowExtensionArgs& InArgs,
	TArray<FPropertyRowExtensionButton>& OutExtensions)
{
	auto& [Icon, Label, ToolTip, UIAction] = OutExtensions.AddDefaulted_GetRef();
	Icon = TAttribute<FSlateIcon>::Create([Handle = InArgs.PropertyHandle, this]()
	{
		return GetCommentIcon(Handle);
	});

	Label = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FBlueprintPropertyCommentEditorModule::GetCommentLabel, InArgs.PropertyHandle));
	ToolTip = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FBlueprintPropertyCommentEditorModule::GetCommentTooltip, InArgs.PropertyHandle));

	UIAction = FUIAction(
		FExecuteAction::CreateRaw(this, &FBlueprintPropertyCommentEditorModule::OnClickComment, InArgs.PropertyHandle),
		FCanExecuteAction::CreateRaw(this, &FBlueprintPropertyCommentEditorModule::CanClickComment, InArgs.PropertyHandle),
		FGetActionCheckState::CreateLambda([](){ return ECheckBoxState::Undetermined; }),
		FIsActionButtonVisible::CreateRaw(this, &FBlueprintPropertyCommentEditorModule::CanClickComment, InArgs.PropertyHandle)
	);
}

void FBlueprintPropertyCommentEditorModule::RegisterAssetRegistryActions()
{
	const FAssetRegistryModule& Module = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	Module.Get().OnAssetRemoved().AddRaw(this, &FBlueprintPropertyCommentEditorModule::HandleOnAssetRemoved);
}

void FBlueprintPropertyCommentEditorModule::DeregisterAssetRegistryActions()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetRegistry"))
	{
		const FAssetRegistryModule& Module = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		Module.Get().OnAssetRemoved().RemoveAll(this);
	}
}

void FBlueprintPropertyCommentEditorModule::HandleOnAssetRemoved(const FAssetData& AssetData)
{
	CloseCurrentOverlayWidget();
}

FSlateIcon FBlueprintPropertyCommentEditorModule::GetCommentIcon(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	FName BrushName{"NoBrush"};

	const UBlueprint* Blueprint = GetBlueprintFromPropertyHandle(PropertyHandle);
	if (IsValid(Blueprint))
	{
		const bool bHasComment = BlueprintHasComment(Blueprint, PropertyHandle);
		BrushName = bHasComment ? "Icons.Comment" : "Icons.Toolbar.Details";
	}
	return FSlateIcon(FAppStyle::Get().GetStyleSetName(), BrushName);
}

FText FBlueprintPropertyCommentEditorModule::GetCommentLabel(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	return LOCTEXT("AddComment", "Add Comment");
}

FText FBlueprintPropertyCommentEditorModule::GetCommentTooltip(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	static const FText AddComment = LOCTEXT("AddCommentTooltip", "Click to add comment");

	const UBlueprint* Blueprint = GetBlueprintFromPropertyHandle(PropertyHandle);
	if (!IsValid(Blueprint))
	{
		return FText{};
	}

	const UPropertyCommentExtension* Extension = UPropertyCommentExtension::GetPropertyCommentExtension(Blueprint);
	if (!IsValid(Extension))
	{
		return AddComment;
	}

	FText Comment;
	const FName PropertyKey = FName{GetPropertyPathRecursive(PropertyHandle)};
	const bool bHasComment = Extension->GetComment(PropertyKey, Comment);
	return bHasComment ? Comment : AddComment;
}

void FBlueprintPropertyCommentEditorModule::OnClickComment(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (!ActiveWindow)
	{
		return;
	}

	// Close any already opened widgets
	CloseCurrentOverlayWidget();

	UBlueprint* Blueprint = GetBlueprintFromPropertyHandle(PropertyHandle);
	check(IsValid(Blueprint));

	FText InitialComment;
	const FName PropertyKey = FName{GetPropertyPathRecursive(PropertyHandle)};
	const UPropertyCommentExtension* Extension = UPropertyCommentExtension::GetPropertyCommentExtension(Blueprint);
	if (IsValid(Extension))
	{
		Extension->GetComment(PropertyKey, InitialComment);
	}

	ActiveWindow->AddOverlaySlot()
	[
		SAssignNew(CurrentOverlayWidget, SCommentEditOverlay)
		.ParentWindow(ActiveWindow)
		.Content(InitialComment)
		.OnConfirmClicked_Lambda([=](FText Comment)
		{
			CloseCurrentOverlayWidget();

			// Check if it's valid low level in case the blueprint had already been deleted
			if (IsValid(Blueprint) && Blueprint->IsValidLowLevel())
			{
				UPropertyCommentExtension* Ext = UPropertyCommentExtension::GetOrCreatePropertyCommentExtension(Blueprint);
				check(IsValid(Ext));

				Ext->AddComment(PropertyKey, Comment);
				Blueprint->MarkPackageDirty();
			}
		})
		.OnCancelClicked_Lambda([this]()
		{
			CloseCurrentOverlayWidget();
		})
	];
}

bool FBlueprintPropertyCommentEditorModule::CanClickComment(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	const UBlueprint* Blueprint = GetBlueprintFromPropertyHandle(PropertyHandle);
	return IsValid(Blueprint);
}

void FBlueprintPropertyCommentEditorModule::CloseCurrentOverlayWidget()
{
	if (CurrentOverlayWidget)
	{
		TWeakPtr<SWindow> ParentWindow = CurrentOverlayWidget->GetParentWindow();
		if (ParentWindow.IsValid())
		{
			ParentWindow.Pin()->RemoveOverlaySlot(CurrentOverlayWidget.ToSharedRef());
		}
	}

	CurrentOverlayWidget = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueprintPropertyCommentEditorModule, BlueprintPropertyCommentEditor)