// Uris - All Rights Reserved

#include "OptionsDataSortingHelper.h"

#include "Settings/UserSettingTypes.h"

namespace
{
bool SortSettingSiblings(const FUserSettingDefinition& Left, const FUserSettingDefinition& Right)
{
	if (Left.SortOrder != Right.SortOrder)
	{
		return Left.SortOrder < Right.SortOrder;
	}

	if (Left.SettingId != Right.SettingId)
	{
		return Left.SettingId.LexicalLess(Right.SettingId);
	}

	return static_cast<uint8>(Left.NativeSetting) < static_cast<uint8>(Right.NativeSetting);
}

void AppendDefinitionAndChildren(
	const FUserSettingDefinition* Definition,
	const TMap<FName, TArray<const FUserSettingDefinition*>>& ChildrenByParentId,
	TSet<const FUserSettingDefinition*>& VisitedDefinitions,
	TArray<const FUserSettingDefinition*>& SortedDefinitions)
{
	if (!Definition || VisitedDefinitions.Contains(Definition))
	{
		return;
	}

	VisitedDefinitions.Add(Definition);
	SortedDefinitions.Add(Definition);

	const TArray<const FUserSettingDefinition*>* Children = ChildrenByParentId.Find(Definition->SettingId);
	if (!Children)
	{
		return;
	}

	for (const FUserSettingDefinition* Child : *Children)
	{
		AppendDefinitionAndChildren(Child, ChildrenByParentId, VisitedDefinitions, SortedDefinitions);
	}
}
}

void FOptionsDataSortingHelper::SortDefinitionsByHierarchy(
	TArray<const FUserSettingDefinition*>& Definitions)
{
	Definitions.RemoveAll([](const FUserSettingDefinition* Definition)
	{
		return Definition == nullptr;
	});

	TSet<FName> AuthoredIds;
	for (const FUserSettingDefinition* Definition : Definitions)
	{
		if (!Definition->SettingId.IsNone())
		{
			AuthoredIds.Add(Definition->SettingId);
		}
	}

	TArray<const FUserSettingDefinition*> RootDefinitions;
	TMap<FName, TArray<const FUserSettingDefinition*>> ChildrenByParentId;
	for (const FUserSettingDefinition* Definition : Definitions)
	{
		if (Definition->ParentSettingId.IsNone() || !AuthoredIds.Contains(Definition->ParentSettingId))
		{
			RootDefinitions.Add(Definition);
		}
		else
		{
			ChildrenByParentId.FindOrAdd(Definition->ParentSettingId).Add(Definition);
		}
	}

	RootDefinitions.Sort(SortSettingSiblings);
	for (TPair<FName, TArray<const FUserSettingDefinition*>>& Pair : ChildrenByParentId)
	{
		Pair.Value.Sort(SortSettingSiblings);
	}

	TArray<const FUserSettingDefinition*> SortedDefinitions;
	SortedDefinitions.Reserve(Definitions.Num());
	TSet<const FUserSettingDefinition*> VisitedDefinitions;

	for (const FUserSettingDefinition* RootDefinition : RootDefinitions)
	{
		AppendDefinitionAndChildren(
			RootDefinition,
			ChildrenByParentId,
			VisitedDefinitions,
			SortedDefinitions);
	}

	// Cyclic parent references have no root. Keep them visible in deterministic order;
	// the registry will attach them to the tab when it cannot resolve a collection parent.
	TArray<const FUserSettingDefinition*> UnvisitedDefinitions = Definitions.FilterByPredicate(
		[&VisitedDefinitions](const FUserSettingDefinition* Definition)
		{
			return !VisitedDefinitions.Contains(Definition);
		});
	UnvisitedDefinitions.Sort(SortSettingSiblings);
	for (const FUserSettingDefinition* Definition : UnvisitedDefinitions)
	{
		AppendDefinitionAndChildren(
			Definition,
			ChildrenByParentId,
			VisitedDefinitions,
			SortedDefinitions);
	}

	Definitions = MoveTemp(SortedDefinitions);
}
