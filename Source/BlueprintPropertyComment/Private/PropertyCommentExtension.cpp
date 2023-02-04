// Fill out your copyright notice in the Description page of Project Settings.


#include "PropertyCommentExtension.h"

#if WITH_EDITORONLY_DATA

UPropertyCommentExtension* UPropertyCommentExtension::GetPropertyCommentExtension(const UBlueprint* Blueprint)
{
	check(IsValid(Blueprint));
	const TObjectPtr<UBlueprintExtension>* FindResult = Blueprint->Extensions.FindByPredicate(
	[](const TObjectPtr<UBlueprintExtension> Ext)
	{
		return IsValid(Ext) && Ext->IsA<UPropertyCommentExtension>();
	});

	if (FindResult)
	{
		return CastChecked<UPropertyCommentExtension>(*FindResult);
	}

	return nullptr;
}

UPropertyCommentExtension* UPropertyCommentExtension::GetOrCreatePropertyCommentExtension(UBlueprint* Blueprint)
{
	check(IsValid(Blueprint));

	UPropertyCommentExtension* Extension = GetPropertyCommentExtension(Blueprint);
	if (!IsValid(Extension))
	{
		Extension = NewObject<UPropertyCommentExtension>(Blueprint);
		Blueprint->Extensions.Add(Extension);
		Blueprint->MarkPackageDirty();
	}

	return Extension;
}

bool UPropertyCommentExtension::HasComment(const FName& PropertyKey) const
{
	return Comments.Contains(PropertyKey);
}

bool UPropertyCommentExtension::GetComment(const FName& PropertyKey, FText& OutComment) const
{
	const FText* FindResult = Comments.Find(PropertyKey);
	if (FindResult)
	{
		OutComment = *FindResult;
	}
	return FindResult != nullptr;
}

void UPropertyCommentExtension::AddComment(const FName& PropertyKey, FText Comment)
{
	if (Comments.Contains(PropertyKey))
	{
		Comments[PropertyKey] = MoveTemp(Comment);
	}
	else
	{
		Comments.Emplace(PropertyKey, MoveTemp(Comment));
	}
}

void UPropertyCommentExtension::RemoveComment(const FName& PropertyKey)
{
	Comments.Remove(PropertyKey);
}

#endif