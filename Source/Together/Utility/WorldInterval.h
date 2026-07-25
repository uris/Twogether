#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"

class UWorld;

/**
 * A JavaScript-style repeating interval/timeout backed by Unreal's world timer manager.
 *
 * Stores the result in an FWorldIntervalPtr and call ClearInterval(Ptr) to stop
 * the callback and reset the pointer.
 */
class TOGETHER_API FWorldInterval : public TSharedFromThis<FWorldInterval>
{
public:
	// Define FCallback as a type capable of storing any no-argument function that returns nothing
	using FCallback = TFunction<void()>;

	/**
	 * Starts a repeating callback.
	 *
	 * @param WorldContextObject An object belonging to the world that owns the timer.
	 * @param Callback           The function to invoke.
	 * @param IntervalSeconds    Delay between invocations, measured in world seconds.
	 * @param DelaySeconds Delay before the first callback invocation.
	 * @param bRepeats Interval behaves like a timeout (repeats = false) or as an interval.
	 * @return The interval handle, or nullptr if the arguments are invalid.
	 */
	static TSharedPtr<FWorldInterval> SetInterval(
		const UObject* WorldContextObject,
		FCallback Callback,
		float IntervalSeconds,
		float DelaySeconds = 0.0f,
		bool bRepeats = true);

	// FWorldInterval destructor
	~FWorldInterval();

	// Prevent an FWorldInterval object from being copied or re-assigned
	FWorldInterval(const FWorldInterval&) = delete; // delete the copy constructor
	FWorldInterval& operator=(const FWorldInterval&) = delete; // delete the re-assignment operator

	/** Stops this interval. Calling Clear more than once is safe. */
	void Clear();

	/** Stops an interval and resets the supplied pointer to nullptr. */
	static void ClearInterval(TSharedPtr<FWorldInterval>& Interval);

	/** True while the repeating world timer is registered. */
	bool IsActive() const;

private:
	FWorldInterval(UWorld* InWorld, FCallback&& InCallback);

	void Start(float IntervalSeconds, float DelaySeconds, bool bRepeats = true);
	void Execute();

	TWeakObjectPtr<UWorld> World;
	FTimerHandle TimerHandle;
	FCallback Callback;
	bool bActive = false;
};

// Define FWorldIntervalPtr as a shared pointer of type FWorldInterval
using FWorldIntervalPtr = TSharedPtr<FWorldInterval>;
