// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SharedTypes/SharedTypes.h"
#include "MotionValue.generated.h"

/**
 * Runtime state for a reusable float transition.
 * The owner supplies DeltaTime and decides how the resulting value is applied.
 */
USTRUCT(BlueprintType)
struct TOGETHER_API FMotionValue
{
	GENERATED_BODY()

public:
	void Configure(float InStartValue, float InEndValue, const FTransition& InTransition);
	void Play();
	void Stop(bool bResetToStart = true);
	void Restart();

	/**
	 * reverses direction. when bUseCurrentValue is false, restarts from the
	 * opposite original endpoint. when true, continues from the current value
	 * toward the opposite endpoint without jumping.
	 */
	void Reverse(bool bUseCurrentValue = false);

	/** advance the transition. Returns true when CurrentValue changed. */
	bool Tick(float DeltaTime);

	float GetValue() const
	{
		return CurrentValue;
	}

	float GetStartValue() const
	{
		return StartValue;
	}

	float GetEndValue() const
	{
		return EndValue;
	}

	bool IsPlaying() const
	{
		return bPlaying;
	}

	bool IsComplete() const
	{
		return bComplete;
	}

	/** True only on the tick where a cycle exits its delay and begins moving. */
	bool DidCycleStart() const
	{
		return bCycleStartedThisTick;
	}

	/** True only on the tick where the complete finite transition finishes. */
	bool DidComplete() const
	{
		return bCompletedThisTick;
	}

private:
	float EvaluateEasedProgress(float Progress) const;
	void CompleteCycle();

	UPROPERTY()
	FTransition Transition;

	float StartValue = 0.0f;
	float EndValue = 1.0f;
	float ConfiguredStartValue = 0.0f;
	float ConfiguredEndValue = 1.0f;
	float CurrentValue = 0.0f;
	float ActiveDuration = 0.25f;
	float CycleElapsed = 0.0f;
	int32 CompletedCycles = 0;
	bool bReversed = false;
	bool bPlaying = false;
	bool bComplete = false;
	bool bCycleStarted = false;
	bool bCycleStartedThisTick = false;
	bool bCompletedThisTick = false;
};
