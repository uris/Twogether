// Uris - All Rights Reserved


#include "ListItemDataObject_IntEnum.h"

bool UListItemDataObject_IntEnum::AddDynamicSetting(const EIntEnumType InEnumType)
{
	if (!CreateIntEnumSettings(InEnumType, GetDataId()))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Unable to create integer enum settings for '%s' using enum type '%s'."),
			*GetDataId().ToString(),
			*StaticEnum<EIntEnumType>()->GetNameStringByValue(static_cast<int64>(InEnumType)));
		return false;
	}

	return true;
}

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

bool UListItemDataObject_IntEnum::CreateIntEnumSettings(const EIntEnumType InEnumType, const FName& InSettingDataId)
{
	// reset settings array
	Settings.Reset();

	// create available settings based on the enum type
	switch (InEnumType)
	{
		case EIntEnumType::WindowMode:
		{
			Settings = GenerateSettingsArray<EWindowMode::Type>(InSettingDataId);
			return !Settings.IsEmpty();
		}
		case EIntEnumType::None:
		case EIntEnumType::Resolution:
		default:
		{
			Settings.Reset();
			return false;
		}
	}
}

template <typename EnumType>
TArray<FStringSetting> UListItemDataObject_IntEnum::GenerateSettingsArray(const FName& InSettingDataId)
{
	TArray<FStringSetting> SettingsArray;

	static_assert(TIsEnum<EnumType>::Value, "EnumType must be an enum");

	const UEnum* Enum = StaticEnum<EnumType>();

	if (!ensure(Enum))
	{
		UE_LOG(LogTemp, Warning, TEXT("Not a reflected enum"))
		return SettingsArray;
	}

	const int32 NumEntries = Enum->NumEnums();

	for (int32 Index = 0; Index < NumEntries; ++Index)
	{

		// explicitly ignore a generated sentinel on reflected types
		if (Index == NumEntries - 1 &&
		    Enum->GetValueByIndex(Index) == Enum->GetMaxEnumValue())
		{
			continue;
		}

		// explicitly ignore hidden values
		bool bIsHidden = false;
#if WITH_METADATA
		bIsHidden = Enum->HasMetaData(TEXT("Hidden"), Index);
#endif
		if (bIsHidden)
		{
			continue;
		}

		const int64 EnumValue = Enum->GetValueByIndex(Index);
		// const FString EnumName = Enum->GetNameStringByIndex(Index);
		const FText EnumDisplayName = Enum->GetDisplayNameTextByIndex(Index);

		FStringSetting SettingEntry;
		SettingEntry.SettingDataId = FName(InSettingDataId);
		SettingEntry.DisplayName = EnumDisplayName;
		SettingEntry.Value = LexToString(EnumValue);
		SettingsArray.Add(SettingEntry);
	}

	return SettingsArray;
}
