// Uris - All Rights Reserved


#include "Widget_TabList.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Widgets/Components/UICommonButtonBase.h"
#include "CommonActionWidget.h"

void UWidget_TabList::NativePreConstruct()
{
	Super::NativePreConstruct();

#if WITH_EDITOR
	if (IsDesignTime())
	{
		CreateTabsEditorOnly();
	}
#endif

	// *****
	// ** RUNTIME ONLY
	// *****
	if (IsDesignTime())
	{
		return;
	}

	// provide the next / previous common action widget access to relevant input data
	if (NextAction)
	{
		NextAction->SetInputAction(NextTabInputActionData);
	}

	if (PreviousAction)
	{
		PreviousAction->SetInputAction(PreviousTabInputActionData);
	}

}


// *****
// ** RUN-TIME CREATE TAB
// *****
void UWidget_TabList::HandleTabCreation_Implementation(FName TabNameID, UCommonButtonBase* TabButton)
{
	Super::HandleTabCreation_Implementation(TabNameID, TabButton);

	if (!TabButton || !TabHolder)
	{
		return;
	}

	if (UUICommonButtonBase* ButtonToCreate = Cast<UUICommonButtonBase>(TabButton))
	{
		// add the tabs buttons to the horizontal slot and configure padding
		UHorizontalBoxSlot* TabButtonSlot = TabHolder->AddChildToHorizontalBox(ButtonToCreate);
		if (TabHolder)
		{
			TabButtonSlot->SetPadding(TabButtonSlotPadding);
		}
	}

}

// *****
// ** EDITOR CREATE TAB
// *****
#if WITH_EDITOR
void UWidget_TabList::CreateTabsEditorOnly()
{
	if (EditorTabCountIsValid() && TabEntryWidgetClassIsValid())
	{
		GenerateEditorTabs();
	}

}

void UWidget_TabList::GenerateEditorTabs()
{
	if (!TabHolder || !IsDesignTime())
	{
		return;
	}

	TabHolder->ClearChildren();
	for (int i = 0; i < GetEditorTabCount(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tab Add"));
		AddTabInEditor();
	}

}

void UWidget_TabList::AddTabInEditor()
{
	if (!TabEntryWidgetClassIsValid() || !TabHolder || !IsDesignTime())
	{
		return;
	}

	// create tab button
	UUICommonButtonBase* TabButton = CreateWidget<
		UUICommonButtonBase>(this, GetTabEntryWidgetClass());

	if (!TabButton)
	{
		return;
	}

	if (UUICommonButtonBase* ButtonToCreate = Cast<UUICommonButtonBase>(TabButton))
	{
		UHorizontalBoxSlot* TabButtonSlot = TabHolder->AddChildToHorizontalBox(ButtonToCreate);
		if (TabHolder)
		{
			TabButtonSlot->SetPadding(TabButtonSlotPadding);
		}
	}

}
#endif
