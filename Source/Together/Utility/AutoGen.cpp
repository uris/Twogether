#include "AutoGen.h"

#include "SharedTypes/SharedTypes.h"

FText UAutoGen::GenerateRandomWords(const FName& DataTableName,
                                    const int32 WordCount,
                                    const FString& Separator,
                                    const int32 MaxWordLength)
{
	const FText OutText = FText::GetEmpty();

	// get game settings
	const UTogetherSettings* Settings = GetDefault<UTogetherSettings>();

	// get the data table
	const TSoftObjectPtr<UDataTable>* FoundTable = Settings->GameData.Find(DataTableName);

	// return empty string if data not found
	if (!FoundTable)
	{
		return OutText;
	}

	// load the data table
	const UDataTable* DataTable = FoundTable->LoadSynchronous();

	// return empty string if no data table
	if (!DataTable)
	{
		return OutText;
	}

	// set up output
	TArray<FString> OutWordArray;
	TArray<FWordListStruct*> AllRows;
	const FString ContextString;

	// read all rows
	DataTable->GetAllRows<FWordListStruct>(ContextString, AllRows);

	// loop each row and add word to the words array
	for (const FWordListStruct* Row : AllRows)
	{
		if (Row && Row->Words.Len() <= MaxWordLength)
		{
			OutWordArray.Add(Row->Words);
		}
	}

	// it its empty, return empty string
	if (OutWordArray.IsEmpty())
	{
		return OutText;
	}

	// Convert GUID into a deterministic integer seed
	// A GUID has 4 uint32 fields (A, B, C, D). XOR them to create a combined seed.
	const FGuid Guid = FGuid::NewGuid();
	const uint32 CombinedSeed = Guid.A ^ Guid.B ^ Guid.C ^ Guid.D;

	// Initialize Unreal's random stream with deterministic seed
	const FRandomStream DeterministicStream(CombinedSeed);

	// Select random words using the stream - concatenate
	FString SessionString = TEXT("");
	for (int32 i = 0; i < WordCount; ++i)
	{
		const int32 RandomIndex = DeterministicStream.RandRange(0, OutWordArray.Num() - 1);
		SessionString += OutWordArray[RandomIndex];

		if (i < WordCount - 1)
		{
			SessionString += Separator;
		}
	}

	return FText::FromString(SessionString);
}
