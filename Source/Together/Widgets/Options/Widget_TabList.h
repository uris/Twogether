// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Components/UICommonTabListWidgetBase.h"
#include "Widget_TabList.generated.h"

class UHorizontalBox;
class UCommonActionWidget;

/**
 *
 */
UCLASS()
class TOGETHER_API UWidget_TabList : public UUICommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tab List Settings")
	FMargin TabButtonSlotPadding = FMargin(0.0f, 0.0f, 0.0f, 0.0f);

protected:
	virtual void NativePreConstruct() override;

	virtual void HandleTabCreation_Implementation(FName TabNameID, UCommonButtonBase* TabButton) override;

private:
#if WITH_EDITOR
	void CreateTabsEditorOnly();
	void GenerateEditorTabs();
	void AddTabInEditor();
#endif

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> TabHolder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActionWidget> PreviousAction;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActionWidget> NextAction;
};
