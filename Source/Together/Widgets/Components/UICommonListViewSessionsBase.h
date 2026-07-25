// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UICommonTextBase.h"
#include "Components/ListView.h"
#include "Subsystems/Multiplayer/MultiplayerSubsystem.h"
#include "UICommonListViewSessionsBase.generated.h"

/**
* UObject representation of list data struct required by UListView
*/
UCLASS(BlueprintType)
class TOGETHER_API USessionListItemObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "List Item")
	FTogetherSessionItem Data;
};

/**
 * The actual list view wrapper
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSessionListItemClicked,
	FTogetherSessionItem,
	ItemData);

UCLASS()
class TOGETHER_API UUICommonListViewSessionsBase : public UListView
{
	GENERATED_BODY()

public:
	// broadcast custom session click event
	UPROPERTY(BlueprintAssignable, Category = "List View")
	FOnSessionListItemClicked OnSessionListItemClicked;

	// expose setting array of session items
	UFUNCTION(BlueprintCallable, Category = "List View")
	void SetGameSessions(const TArray<FTogetherSessionItem>& InItems);

	// reset list view
	UFUNCTION(BlueprintCallable, Category = "UI")
	void DeactivateListView();

	// track initial load of results
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "List View")
	int32 LoadIndex = 0;

	// track initial load of results
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "List View")
	bool bFocusFirstItemOnLoad = true;

	// expose getting array of session items
	UFUNCTION(BlueprintPure, Category = "List View")
	TArray<FTogetherSessionItem> GetGameSessions() const
	{
		return StructItems;
	}

protected:
	// override list item click
	virtual void OnItemClickedInternal(UObject* Item) override;

private:
	// cache data items
	UPROPERTY(Transient)
	TArray<FTogetherSessionItem> StructItems;

	// UPROPERTY keeps the wrapper objects alive during garbage collection.
	UPROPERTY(Transient)
	TArray<TObjectPtr<USessionListItemObject>> ItemObjects;

};


/**
 * List view entry widget
 */
UENUM(BlueprintType)
enum class EItemState : uint8
{
	Hovered UMETA(DisplayName = "Hovered"),
	Selected UMETA(DisplayName = "Selected"),
	Normal UMETA(DisplayName = "Normal"),
};

UCLASS(Abstract, Blueprintable)
class TOGETHER_API UUISessionListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUICommonTextBase> SessionName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUICommonTextBase> SessionId;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUICommonTextBase> IsFull;

	/** Bound Widgets - Audio */

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Sound",
		meta=(DisplayThumbnail = "true", AllowedClasses = "/Script/Engine.SoundBase",
			AllowPrivateAccess="true"))
	USoundBase* ClickSound = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Sound",
		meta=(AllowPrivateAccess="true", ClampMin="0.0", UIMin="0.0", UIMax="2.0"))
	float ClickVolume = 1.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Sound",
		meta=(DisplayThumbnail = "true", AllowedClasses = "/Script/Engine.SoundBase",
			AllowPrivateAccess="true"))
	USoundBase* HoverSound = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Sound",
		meta=(AllowPrivateAccess="true", ClampMin="0.0", UIMin="0.0", UIMax="2.0"))
	float HoverVolume = 1.0f;

	/** Bound Widgets - Audio */


	// special callable allowing method to be called from BP as event, but also from c++
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "List Item")
	void HandleSessionClick();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "List Item|Style")
	TSubclassOf<UCommonTextStyle> NormalTextStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "List Item|Style")
	TSubclassOf<UCommonTextStyle> SelectedTextStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "List Item|Style")
	TSubclassOf<UCommonTextStyle> HoverTextStyle;

	// Override on object set
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// override initial setup
	virtual void NativeConstruct() override;

	// override selection states to apply text styles
	// note that "selection" is no "click", it means it has focus/hover but not necessarily clicked
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	void ApplySelectionStyle(EItemState ItemState) const;
	bool bSessionSelected = false;
	bool bSessionHovered = false;

	// Event triggered on item data set
	UFUNCTION(BlueprintImplementableEvent, Category = "List Item", meta = (DisplayName = "On List Item Data Set"))
	void OnListItemDataSet(const FTogetherSessionItem& Data);
};
