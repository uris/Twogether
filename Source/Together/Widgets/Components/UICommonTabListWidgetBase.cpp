// Uris - All Rights Reserved


#include "Widgets/Components/UICommonTabListWidgetBase.h"

#if WITH_EDITOR
#include "Editor/WidgetCompilerLog.h"
#endif

#include "Utility/Debug.h"
#include "Widgets/Components/UICommonButtonBase.h"

void UUICommonTabListWidgetBase::RequestRegisterTab(const FName& InTabId, const FText& InTabDisplayName)
{
	RegisterTab(InTabId, TabButtonEntryWidgetClass, nullptr);

	UUICommonButtonBase* FoundButton = Cast<UUICommonButtonBase>(GetTabButtonBaseByID(InTabId));

	if (FoundButton)
	{
		FoundButton->SetButtonText(InTabDisplayName);
	}
}

#if WITH_EDITOR
int32 UUICommonTabListWidgetBase::EditorTabCountIsValid() const
{
	return DebugEditorPreviewTabCount > 0;
}

bool UUICommonTabListWidgetBase::TabEntryWidgetClassIsValid() const
{
	return TabButtonEntryWidgetClass && TabButtonEntryWidgetClass->IsChildOf(UUICommonButtonBase::StaticClass());
}

int32 UUICommonTabListWidgetBase::GetEditorTabCount() const
{
	return DebugEditorPreviewTabCount;
}

TSubclassOf<UUICommonButtonBase> UUICommonTabListWidgetBase::GetTabEntryWidgetClass() const
{
	return TabButtonEntryWidgetClass;
}

void UUICommonTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(
			TEXT("The VariableTabButtonEntrWidgetClass has no valid entry specified") +
			GetClass()->GetName() +
			TEXT(" needs a valid entry widget class to function properly")
			));
	}
}

void UUICommonTabListWidgetBase::HandleTabSelected(FName TabId)
{
	Debug::Print(TabId.ToString() + " Selected");
}
#endif
