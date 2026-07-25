#include "WorldInterval.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

FWorldIntervalPtr FWorldInterval::SetInterval(
	const UObject* WorldContextObject,
	FCallback Callback,
	const float IntervalSeconds,
	const float DelaySeconds,
	const bool bRepeats)
{
	// protect for valid args
	if (!IsValid(WorldContextObject) || !Callback || IntervalSeconds <= 0.0f || !GEngine || DelaySeconds < 0.0f)
	{
		return nullptr;
	}

	// get resolved world for the context object timer is attached to
	UWorld* ResolvedWorld = GEngine->GetWorldFromContextObject(
		WorldContextObject,
		EGetWorldErrorMode::ReturnNull);

	// protect for a valid world
	if (!IsValid(ResolvedWorld))
	{
		return nullptr;
	}

	// create a shareable pointer to FWorldInterval with passed in args
	FWorldIntervalPtr Interval = MakeShareable(
		new FWorldInterval(ResolvedWorld, MoveTemp(Callback)));

	// call interval start
	Interval->Start(IntervalSeconds, DelaySeconds, bRepeats);

	// return the interval pointer
	return Interval;
}

// constructor takes in and internally assigns World/Callback from the args
FWorldInterval::FWorldInterval(UWorld* InWorld, FCallback&& InCallback)
	: World(InWorld)
	  , Callback(MoveTemp(InCallback)) {}

// destructor calls clear to nullify pointers
FWorldInterval::~FWorldInterval()
{
	Clear();
}

// start the timer
void FWorldInterval::Start(const float IntervalSeconds, const float DelaySeconds, const bool bRepeats)
{
	// protect for a valid world and callback
	UWorld* ResolvedWorld = World.Get();
	if (!ResolvedWorld || !Callback)
	{
		return;
	}

	const TWeakPtr<FWorldInterval> WeakThis = AsShared();
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateLambda([WeakThis, bRepeats]()
	{
		if (const FWorldIntervalPtr Interval = WeakThis.Pin())
		{
			Interval->Execute();
		}
	});

	bActive = true;

	ResolvedWorld->GetTimerManager().SetTimer(
		TimerHandle,
		MoveTemp(TimerDelegate),
		IntervalSeconds,
		bRepeats,
		DelaySeconds);
}

// execute the callback if the timer is active
void FWorldInterval::Execute()
{
	if (bActive && Callback)
	{
		Callback();
	}
}

// clear and release timer variables and pointers
void FWorldInterval::Clear()
{
	if (UWorld* ResolvedWorld = World.Get())
	{
		ResolvedWorld->GetTimerManager().ClearTimer(TimerHandle);
	}

	TimerHandle.Invalidate();
	Callback = nullptr;
	bActive = false;
	World.Reset();
}

// stop the interval
void FWorldInterval::ClearInterval(FWorldIntervalPtr& Interval)
{
	if (Interval)
	{
		Interval->Clear();
		Interval.Reset();
	}
}

// check if TimerHandle is an active timer
bool FWorldInterval::IsActive() const
{
	if (!bActive)
	{
		return false;
	}

	const UWorld* ResolvedWorld = World.Get();
	return ResolvedWorld && ResolvedWorld->GetTimerManager().IsTimerActive(TimerHandle);
}
