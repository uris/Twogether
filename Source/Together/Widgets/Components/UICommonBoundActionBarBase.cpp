// Uris - All Rights Reserved


#include "Widgets/Components/UICommonBoundActionBarBase.h"

void UUICommonBoundActionBarBase::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	SetEntrySpacing({Gap, 0.0f});
}
