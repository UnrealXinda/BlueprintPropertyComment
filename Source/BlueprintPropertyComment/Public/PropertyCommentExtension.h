// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/BlueprintExtension.h"
#include "PropertyCommentExtension.generated.h"

/**
 * 
 */
UCLASS()
class BLUEPRINTPROPERTYCOMMENT_API UPropertyCommentExtension final : public UBlueprintExtension
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
public:
	static UPropertyCommentExtension* GetPropertyCommentExtension(const UBlueprint* Blueprint);
	static UPropertyCommentExtension* GetOrCreatePropertyCommentExtension(UBlueprint* Blueprint);
	static void TryAddPropertyComment(UBlueprint* Blueprint, const FName& PropertyKey, FText Comment);

	bool HasComment(const FName& PropertyKey) const;
	bool GetComment(const FName& PropertyKey, FText& OutComment) const;
	void AddComment(const FName& PropertyKey, FText Comment);
	void RemoveComment(const FName& PropertyKey);

protected:
	UPROPERTY()
	TMap<FName, FText> Comments;
#endif
};