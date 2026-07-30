// Uris - All Rights Reserved


#include "ListItemDataObject_IntEnum.h"

void UListItemDataObject_IntEnum::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	if (Settings.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Unable to initialize integer enum setting '%s': no available values were generated."),
			*GetDataId().ToString());
		Super::OnDataObjectInitialized();
		return;
	}

	const FString ConfiguredDefault = GetDefaultValueAsString();
	const bool bHasValidDefault = Settings.ContainsByPredicate(
		[&ConfiguredDefault](const FStringSetting& Setting)
		{
			return Setting.Value == ConfiguredDefault;
		});

	if (!bHasValidDefault)
	{
		if (!ConfiguredDefault.IsEmpty())
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Integer enum setting '%s' has unavailable default '%s'; using '%s' instead."),
				*GetDataId().ToString(),
				*ConfiguredDefault,
				*Settings[0].Value);
		}

		// Enum settings persist their underlying integer value, not their display label.
		SetDefaultValueFromString(Settings[0].Value);
	}
}
