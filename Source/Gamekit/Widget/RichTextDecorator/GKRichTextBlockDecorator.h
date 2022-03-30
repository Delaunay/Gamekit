// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateTypes.h"
#include "Framework/Text/TextLayout.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/ITextDecorator.h"
#include "Components/RichTextBlockDecorator.h"
#include "Engine/DataTable.h"

// Generated
#include "GKRichTextBlockDecorator.generated.h"

class ISlateStyle;

// This save the style config
USTRUCT(Blueprintable, BlueprintType)
struct GAMEKIT_API FGKRichRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Appearance)
	FSlateBrush Brush;
};


// This can be in a .cpp file nobody needs to interact with this
class FGKRichStruct : public FRichTextDecorator
{
public:
	FGKRichStruct(URichTextBlock* InOwner, class UGKRichTextBlockDecorator* InDecorator) :
		FRichTextDecorator(InOwner),
		Decorator(InDecorator)
	{}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override {
		return false;
	}

protected:
	/** Override this function if you want to create a unique widget like an image */
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& DefaultTextStyle) const {
		return nullptr;
	}

	/** Override this function if you want to dynamically generate text, optionally changing the style. InOutString will start as the content between tags */
	virtual void CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const {
	
	}

	class UGKRichTextBlockDecorator* Decorator;
};

/**
 * this convert a text block into a widget
 */
UCLASS(Abstract, Blueprintable)
class GAMEKIT_API UGKRichTextBlockDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	UGKRichTextBlockDecorator(const FObjectInitializer& ObjectInitializer) :
		URichTextBlockDecorator(ObjectInitializer)
	{}

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override {
		return MakeShareable(new FGKRichStruct(InOwner, this));
	}

protected:

	FGKRichRow* FindRow(FName TagOrId, bool bWarnIfMissing) {
		return nullptr;
	}

	UPROPERTY(EditAnywhere, Category = Appearance, meta = (RequiredAssetDataTags = "RowStructure=RichImageRow"))
	class UDataTable* ImageSet;
};
