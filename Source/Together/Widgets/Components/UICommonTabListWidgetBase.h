// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "UICommonTabListWidgetBase.generated.h"

class UUICommonButtonBase;
/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	void RequestRegisterTab(const FName& InTabId, const FText& InTabDisplayName);

protected:
	int32 EditorTabCountIsValid() const;
	bool TabEntryWidgetClassIsValid() const;
	int32 GetEditorTabCount() const;
	TSubclassOf<UUICommonButtonBase> GetTabEntryWidgetClass() const;

private:
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Tab List Settings",
		meta=(AllowPrivateAccess="true", ClampMin="1", ClampMax="10"))
	int32 DebugEditorPreviewTabCount = 3;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Tab List Settings",
		meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUICommonButtonBase> TabButtonEntryWidgetClass;

};
