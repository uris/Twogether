// Uris - All Rights Reserved

#include "Subsystems/UI/UISubsystem.h"

#include "CommonInputSubsystem.h"
#include "TogetherUIGameplayTags.h"
#include "UIFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/DeveloperSettings/TogetherSettings.h"
#include "Utility/Debug.h"
#include "Widgets/Base/Widget_ActivatableBase.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Screens/Widget_Confirmation.h"
#include "Sound/SoundBase.h"

// init triggering load of core SFX
void UUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// get all SFX tags for sounds to preload
	const FGameplayTag SoundFXParent = FGameplayTag::RequestGameplayTag("UI.SoundFX");
	if (SoundFXParent.IsValid())
	{
		SoundFXToPreload = UUIFunctionLibrary::GetLeafChildTags(SoundFXParent);
	}

	// trigger async load of the sounds library after the preload list is ready
	const UTogetherSettings* Settings = GetDefault<UTogetherSettings>();
	if (Settings && !Settings->DefaultSoundFX.IsNull())
	{
		StreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
			Settings->DefaultSoundFX.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::SoundFXLibraryDidLoad));
	}
}

// deinit resetting state
void UUISubsystem::Deinitialize()
{

	// reset handles
	if (StreamableHandle.IsValid())
	{
		StreamableHandle->CancelHandle();
		StreamableHandle.Reset();
	}

	// reset sfx load state
	PreloadedSoundFX.Reset();
	bSFXLibraryLoaded = false;
	bCoreSoundFXLoaded = false;
	LoadedSoundFXLibrary = nullptr;

	Super::Deinitialize();
}

// helper to get current input type
ECommonInputType UUISubsystem::GetCurrentInputMode(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		if (!World)
		{
			return ECommonInputType::MouseAndKeyboard;
		}

		const APlayerController* PC = World->GetFirstPlayerController();
		if (!PC)
		{
			return ECommonInputType::MouseAndKeyboard;
		}

		const UCommonInputSubsystem* InputSystem = UCommonInputSubsystem::Get(PC->GetLocalPlayer());
		if (!InputSystem)
		{
			return ECommonInputType::MouseAndKeyboard;
		}

		return InputSystem->GetCurrentInputType();
	}

	return ECommonInputType::MouseAndKeyboard;
}

bool UUISubsystem::IsGamepadInputMode(const UObject* WorldContextObject)
{
	return GetCurrentInputMode(WorldContextObject) == ECommonInputType::Gamepad;
}

// public getter for c++ and BP returning the UI subsystem
UUISubsystem* UUISubsystem::GetUISubsystem(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		return UGameInstance::GetSubsystem<UUISubsystem>(World->GetGameInstance());
	}

	return nullptr;
}

// override creating a UI subsystem on server only where no UI is needed
bool UUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	Super::ShouldCreateSubsystem(Outer);

	// UI is not needed on a dedicated server
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> Classes;
		GetDerivedClasses(GetClass(), Classes);
		return Classes.IsEmpty();
	};

	return false;
}

// register the loaded widget to the subsystem
void UUISubsystem::RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget)
{
	if (InCreatedWidget)
	{
		PrimaryLayout = InCreatedWidget;
	}

}

// return ownership of mouse input type to pc before quit
void UUISubsystem::ResetInputAndQuitGame(UObject* ObjectContext)
{

	const UWorld* World = GEngine->GetWorldFromContextObject(ObjectContext, EGetWorldErrorMode::Assert);
	check(World);

	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	check(LocalPlayer);

	// get input subsystem
	UCommonInputSubsystem* CommonInput = ULocalPlayer::GetSubsystem<UCommonInputSubsystem>(LocalPlayer);

	// reset input mode
	if (CommonInput && CommonInput->GetCurrentInputType() != ECommonInputType::MouseAndKeyboard)
	{
		CommonInput->SetCurrentInputType(ECommonInputType::MouseAndKeyboard);
	}

	// quit game
	if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(World))
	{
		UKismetSystemLibrary::QuitGame(
			World,
			PlayerController,
			EQuitPreference::Quit,
			false
			);
	}

}

// ** push a soft widget to the UI widget stack
void UUISubsystem::PushSoftWidgetToStack(const FGameplayTag& InWidgetStackTag,
                                         TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
                                         TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)>
                                         AsyncPushStateCallback) const
{
	// ensure widget in parameter
	check(!InSoftWidgetClass.IsNull());

	// Soft path to widget
	FSoftObjectPath StreamPath = InSoftWidgetClass.ToSoftObjectPath();

	// define the lambda callback function to run when loading the widget async
	const TFunction<void()> LoadWidgetLambda = ([InWidgetStackTag, InSoftWidgetClass,AsyncPushStateCallback, this]()
	{
		PushSoftWidgetToStackLambda(InWidgetStackTag, InSoftWidgetClass, AsyncPushStateCallback);
	});

	// create a delegate to handle the async load
	TDelegate<void()> AsyncWidgetLoaderHandle = FStreamableDelegate::CreateLambda(LoadWidgetLambda);

	// perform the async widget load
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(StreamPath, AsyncWidgetLoaderHandle);
}

// ** push confirmation modals into the widget stack
void UUISubsystem::PushConfirmationScreenToStack(const EConfirmationScreenType& InScreenType,
                                                 const FText& InScreenTitle,
                                                 const FText& InScreenMessage,
                                                 const FConfirmationButtonLabels& InButtonLabels,
                                                 TFunction<void(EConfirmationButtonType)> ButtonClickedCallback,
                                                 const FGameplayTag& InIconTag,
                                                 const FGameplayTag& InSoundFXTag) const
{

	// create info object based on screen type containing the modal content and actions buttons
	UConfirmationScreenInfoObject* CreatedInfoObject = UConfirmationScreenInfoObject::CreateConfirmationInfoObject(
		InScreenTitle,
		InScreenMessage,
		InButtonLabels,
		InScreenType,
		InIconTag,
		InSoundFXTag);

	// ensure the info object was successfully created
	check(CreatedInfoObject)

	// create the callback lambda since this push happens async
	const TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> PushCallback = [CreatedInfoObject,
			ButtonClickedCallback](const EAsyncPushWidgetState InPushState,
			                       UWidget_ActivatableBase* PushedWidget)
	{
		if (InPushState == EAsyncPushWidgetState::OnBeforePush)
		{
			UWidget_Confirmation* CreatedConfirmScreen = CastChecked<UWidget_Confirmation>(
				PushedWidget);
			CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
		}
	};

	// do the actual async soft push
	PushSoftWidgetToStack(UIGameplayTags::UI_WidgetStack_Modal,
	                      UUIFunctionLibrary::GetFrontEndSoftWidgetClassByTag(UIGameplayTags::UI_Widget_ConfirmScreen),
	                      PushCallback);

}

// helper: get active widget by stack tag name
UCommonActivatableWidgetContainerBase* UUISubsystem::FindWidgetStackByTag(const FGameplayTag& WidgetStackTag) const
{
	if (!PrimaryLayout)
	{
		return nullptr;
	}
	return PrimaryLayout->FindWidgetStackByTag(WidgetStackTag);
}

// helper: modularize the lambda callback into separate function for clearer code and maintainability
void UUISubsystem::PushSoftWidgetToStackLambda(const FGameplayTag& InWidgetStackTag,
                                               const TSoftClassPtr<UWidget_ActivatableBase>& InSoftWidgetClass,
                                               TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)>
                                               AsyncPushStateCallback) const
{
	// ensure the PrimaryLayout widget is available
	check(PrimaryLayout);

	// get the widget class we'll be loading and ensure it exists
	UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
	check(LoadedWidgetClass);

	// get the widget container we'll load the widget content into using the corresponding stack tag
	UCommonActivatableWidgetContainerBase* FoundWidgetStack = PrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
	check(FoundWidgetStack);

	// on adding the widget, use AddWidgets lambda to raise OnBeforePush event with the created widget instance
	// So if there's setup to do on the instance prior to load, there's an opportunity to do so
	UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(LoadedWidgetClass,
		[AsyncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
		{
			AsyncPushStateCallback(EAsyncPushWidgetState::OnBeforePush, &CreatedWidgetInstance);
		});

	// after adding/loading, notify OnAfterLoad with the created/added widget,
	// so if there are knock-on actions, there's a good trigger for them
	AsyncPushStateCallback(EAsyncPushWidgetState::OnAfterPush, CreatedWidget);
}

// once sound library loads, load desired sounds
void UUISubsystem::SoundFXLibraryDidLoad()
{
	// get settings
	const UTogetherSettings* Settings = GetDefault<UTogetherSettings>();
	if (!Settings)
	{
		Debug::Print("Unable to load SoundFX Library");
		return;
	}

	// get SFX library again
	LoadedSoundFXLibrary = Settings->DefaultSoundFX.Get();
	if (!LoadedSoundFXLibrary)
	{
		Debug::Print("Failed to load SoundFX Library");
		return;
	}

	// reset
	StreamableHandle.Reset();
	bSFXLibraryLoaded = true;

	// load sounds
	BeginLoadingCoreSFX();

}

// load core SFX async to make available across widgets
void UUISubsystem::BeginLoadingCoreSFX()
{

	if (!LoadedSoundFXLibrary)
	{
		Debug::Print("SoundFXLibrary unavailable");
		return;
	}

	// reset state and db
	bCoreSoundFXLoaded = false;
	PreloadedSoundFX.Reset();

	// set up a soft pointer to sounds to load based on setting
	TArray<FSoftObjectPath> SoundsToLoad;
	SoundsToLoad.Reserve(SoundFXToPreload.Num());
	Debug::Print("No Item SFX %d", SoundFXToPreload.Num());

	// get each sound by tag and add to assets to load array
	for (const FGameplayTag& SoundTag : SoundFXToPreload)
	{
		const FUISoundFXDefinition* Definition = LoadedSoundFXLibrary->FindSoundDefinition(SoundTag);
		if (!Definition || Definition->Sound.IsNull())
		{
			const FString Msg = FString::Printf(TEXT("No sound def/value for %s"), *SoundTag.ToString());
			Debug::Print(Msg);
			continue;
		}

		// add the sound asset to the assets to load array
		SoundsToLoad.AddUnique(Definition->Sound.ToSoftObjectPath());
	}

	// if no assets, handle loaded and exit
	if (SoundsToLoad.IsEmpty())
	{
		HandleCoreSFXLoaded();
		return;
	}

	// lod assets async
	StreamableHandle =
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SoundsToLoad,
			FStreamableDelegate::CreateUObject(
				this,
				&ThisClass::HandleCoreSFXLoaded));
}

// after SFX load resolve soft to firm, and broadcast load complete
void UUISubsystem::HandleCoreSFXLoaded()
{

	// reset handle
	PreloadedSoundFX.Reset();

	// set up counts
	int32 TotalSFXToLoad = SoundFXToPreload.Num();
	int32 LoadedSFXCount = PreloadedSoundFX.Num();

	// load each sound tag
	if (LoadedSoundFXLibrary)
	{
		for (const FGameplayTag& SoundTag : SoundFXToPreload)
		{
			// get sound definition from loaded SFX library
			const FUISoundFXDefinition* Definition = LoadedSoundFXLibrary->FindSoundDefinition(SoundTag);
			if (!Definition)
			{
				const FString Msg = FString::Printf(TEXT("No sound def for %s"), *SoundTag.ToString());
				Debug::Print(Msg);
				continue;
			}

			// make sure the definition has a sound and is not null before adding it to the preloaded sounds
			if (USoundBase* Sound = Definition->Sound.Get())
			{
				PreloadedSoundFX.Add(SoundTag, Sound);
				LoadedSFXCount++;
			}
			else
			{
				const FString Msg = FString::Printf(TEXT("Unable to preload %s"), *SoundTag.ToString());
				Debug::Print(Msg);
			}
		}
	}

	// reset tracking member variables
	bCoreSoundFXLoaded = true;
	StreamableHandle.Reset();
	SoundFXToPreload.Reset();

	// broadcast load
	FString Msg = FString::Printf(TEXT("Loaded: %d of %d"), LoadedSFXCount, TotalSFXToLoad);
	Debug::Print(Msg);
	OnCoreSoundFXLoaded.Broadcast(TotalSFXToLoad, LoadedSFXCount);
}

// service requests for preloaded by tag
USoundBase* UUISubsystem::GetPreloadedSFX(const FGameplayTag SoundFXTag) const
{
	const TObjectPtr<USoundBase>* Sound = PreloadedSoundFX.Find(SoundFXTag);
	return Sound ? Sound->Get() : nullptr;
}
