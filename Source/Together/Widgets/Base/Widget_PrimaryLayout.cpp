// Uris - All Rights Reserved


#include "Widgets/Base/Widget_PrimaryLayout.h"

#include "Utility/Debug.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InStackTag) const
{
	checkf(WidgetStack.Contains(InStackTag), TEXT("Widget Stack not found: %s"), *InStackTag.ToString());

	return WidgetStack.FindRef(InStackTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(FGameplayTag InStackTag,
                                                UCommonActivatableWidgetContainerBase* InWidgetStack)
{
	if (!IsDesignTime())
	{

		if (!WidgetStack.Contains(InStackTag))
		{
			WidgetStack.Add(InStackTag, InWidgetStack);
			Print(FString::Printf(TEXT("Widget Stack registered: %s"), *InStackTag.ToString()));
		}
	}
}

void UWidget_PrimaryLayout::Print(const FString& Message) const
{
	if (bShowDebugMessages)
	{
		Debug::Print(Message);
	}
}