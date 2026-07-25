#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Settings/TogetherSettings.h"

#include "AutoGen.generated.h"

UCLASS()
class TOGETHER_API UAutoGen : public UObject
{

	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Utility|Random Words")
	static FText GenerateRandomWords(const FName& DataTableName,
	                                 const int32 WordCount = 3,
	                                 const FString& Separator = TEXT("-"),
	                                 const int32 MaxWordLength = 10);
};
