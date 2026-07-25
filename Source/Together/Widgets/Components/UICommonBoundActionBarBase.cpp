// Uris - All Rights Reserved


#include "Widgets/Components/UICommonBoundActionBarBase.h"

#include "Input/CommonBoundActionButtonInterface.h"
#include "Tests/ToolMenusTestUtilities.h"
#include "Widgets/Components/UICommonBoundActionButtonBase.h"

void UUICommonBoundActionBarBase::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	SetEntrySpacing({Gap, 0.0f});
}