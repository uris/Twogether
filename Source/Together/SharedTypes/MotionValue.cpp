// Uris - All Rights Reserved

#include "SharedTypes/MotionValue.h"

void FMotionValue::Configure(
	const float InStartValue,
	const float InEndValue,
	const FTransition& InTransition)
{
	ConfiguredStartValue = InStartValue;
	ConfiguredEndValue = InEndValue;
	StartValue = ConfiguredStartValue;
	EndValue = ConfiguredEndValue;
	Transition = InTransition;
	ActiveDuration = FMath::Max(0.0f, Transition.Duration);
	bReversed = false;
	Stop(true);
}

void FMotionValue::Play()
{
	CycleElapsed = 0.0f;
	CompletedCycles = 0;
	CurrentValue = StartValue;
	bPlaying = true;
	bComplete = false;
	bCycleStarted = false;
	bCycleStartedThisTick = false;
	bCompletedThisTick = false;
}

void FMotionValue::Stop(const bool bResetToStart)
{
	bPlaying = false;
	bComplete = false;
	bCycleStarted = false;
	bCycleStartedThisTick = false;
	bCompletedThisTick = false;
	CycleElapsed = 0.0f;
	CompletedCycles = 0;

	if (bResetToStart)
	{
		CurrentValue = StartValue;
	}
}

void FMotionValue::Restart()
{
	Play();
}

void FMotionValue::Reverse(const bool bUseCurrentValue)
{
	bReversed = !bReversed;
	const float NewEndValue = bReversed ? ConfiguredStartValue : ConfiguredEndValue;

	if (bUseCurrentValue)
	{
		const float FullDistance = FMath::Abs(ConfiguredEndValue - ConfiguredStartValue);
		const float ReverseDistance = FMath::Abs(CurrentValue - NewEndValue);
		const float DistanceRatio = FullDistance > KINDA_SMALL_NUMBER
			? FMath::Clamp(ReverseDistance / FullDistance, 0.0f, 1.0f)
			: 0.0f;

		StartValue = CurrentValue;
		EndValue = NewEndValue;
		ActiveDuration = FMath::Max(0.0f, Transition.Duration) * DistanceRatio;
	}
	else
	{
		StartValue = bReversed ? ConfiguredEndValue : ConfiguredStartValue;
		EndValue = NewEndValue;
		ActiveDuration = FMath::Max(0.0f, Transition.Duration);
	}

	Play();
}

bool FMotionValue::Tick(const float DeltaTime)
{
	bCycleStartedThisTick = false;
	bCompletedThisTick = false;

	if (!bPlaying)
	{
		return false;
	}

	const float PreviousValue = CurrentValue;
	CycleElapsed += FMath::Max(0.0f, DeltaTime);

	const float Delay = CompletedCycles > 0
		? FMath::Max(0.0f, Transition.LoopDelay)
		: FMath::Max(0.0f, Transition.Delay);
	if (CycleElapsed < Delay)
	{
		CurrentValue = StartValue;
		return !FMath::IsNearlyEqual(CurrentValue, PreviousValue);
	}

	if (!bCycleStarted)
	{
		bCycleStarted = true;
		bCycleStartedThisTick = true;
	}

	const float Duration = ActiveDuration;
	if (Duration <= KINDA_SMALL_NUMBER)
	{
		CurrentValue = Transition.bYoYo ? StartValue : EndValue;
		bPlaying = false;
		bComplete = true;
		bCompletedThisTick = true;
		bCycleStarted = false;
		return !FMath::IsNearlyEqual(CurrentValue, PreviousValue);
	}

	const float AnimationElapsed = CycleElapsed - Delay;
	const float CycleDuration = Transition.bYoYo ? Duration * 2.0f : Duration;
	if (AnimationElapsed >= CycleDuration)
	{
		CurrentValue = Transition.bYoYo ? StartValue : EndValue;
		CompleteCycle();
		return !FMath::IsNearlyEqual(CurrentValue, PreviousValue);
	}

	float Alpha;
	if (Transition.bYoYo && AnimationElapsed >= Duration)
	{
		Alpha = 1.0f - EvaluateEasedProgress((AnimationElapsed - Duration) / Duration);
	}
	else
	{
		Alpha = EvaluateEasedProgress(AnimationElapsed / Duration);
	}

	CurrentValue = FMath::Lerp(StartValue, EndValue, Alpha);
	return !FMath::IsNearlyEqual(CurrentValue, PreviousValue);
}

float FMotionValue::EvaluateEasedProgress(const float Progress) const
{
	const float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);

	switch (Transition.TransitionType)
	{
		case ECustomTransitionType::EaseInOut:
			return FMath::InterpEaseInOut(0.0f, 1.0f, ClampedProgress, 2.0f);
		case ECustomTransitionType::EaseIn:
			return FMath::InterpEaseIn(0.0f, 1.0f, ClampedProgress, 2.0f);
		case ECustomTransitionType::EaseOut:
			return FMath::InterpEaseOut(0.0f, 1.0f, ClampedProgress, 2.0f);
		case ECustomTransitionType::Curve:
			return Transition.Curve
				? FMath::Clamp(Transition.Curve->GetFloatValue(ClampedProgress), 0.0f, 1.0f)
				: ClampedProgress;
		case ECustomTransitionType::Linear:
		default:
			return ClampedProgress;
	}
}

void FMotionValue::CompleteCycle()
{
	++CompletedCycles;

	const bool bRepeatIndefinitely = Transition.RepeatTimes == 0;
	const int32 CycleCount = FMath::Max(0, Transition.RepeatTimes) + 1;
	if (!bRepeatIndefinitely && CompletedCycles >= CycleCount)
	{
		bPlaying = false;
		bComplete = true;
		bCompletedThisTick = true;
		bCycleStarted = false;
		return;
	}

	CycleElapsed = 0.0f;
	bCycleStarted = false;
	CurrentValue = StartValue;
}
