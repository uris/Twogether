#include "FileIO.h"

FString FFileIO::ReadFile(const FString& FilePath)
{
	FString FullFilePath = FilePath;

	if (FPaths::IsRelative(FilePath))
	{
		FullFilePath = FPaths::ConvertRelativePathToFull(
			FPaths::ProjectDir(),
			FilePath);
	}

	if (FString OutContents; FFileHelper::LoadFileToString(OutContents, *FilePath))
	{
		return OutContents;
	}

	return FString();

}
