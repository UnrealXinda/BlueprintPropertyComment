// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IBlueprintPropertyCommentEditor.h"

class SCommentEditOverlay;

class FBlueprintPropertyCommentEditorModule final : public IBlueprintPropertyCommentEditorModule
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterDetailRowExtension();
	void DeregisterDetailRowExtension();
	void HandleCreatePropertyRowExtension(const FOnGenerateGlobalRowExtensionArgs& InArgs, TArray<FPropertyRowExtensionButton>& OutExtensions);

	void RegisterAssetRegistryActions();
	void DeregisterAssetRegistryActions();
	void HandleOnAssetRemoved(const FAssetData& AssetData);

	FSlateIcon GetCommentIcon(TSharedPtr<IPropertyHandle> PropertyHandle);
	FText GetCommentLabel(TSharedPtr<IPropertyHandle> PropertyHandle);
	FText GetCommentTooltip(TSharedPtr<IPropertyHandle> PropertyHandle);

	void OnClickComment(TSharedPtr<IPropertyHandle> PropertyHandle);
	bool CanClickComment(TSharedPtr<IPropertyHandle> PropertyHandle);

	void CloseCurrentOverlayWidget();

	TSharedPtr<SCommentEditOverlay> CurrentOverlayWidget;
};