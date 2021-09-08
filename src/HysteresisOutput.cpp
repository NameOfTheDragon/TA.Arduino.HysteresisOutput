/*
 Name:		HysteresisOutput.cpp
 Author:	Tim Long (TimTek)
 Editor:	http://www.visualmicro.com
*/

#include <functional>
#include "HysteresisOutput.h"

void HysteresisOutput::turnOn()
{
	setInputState(true);
}

void HysteresisOutput::turnOff()
{
	setInputState(false);
}

void HysteresisOutput::loop()
{
	// We don't need to take any action if the current state is already equal to the target state.
	if (currentState == targetState)
	{
		// expired timers must be stopped or restarted, or there will be problems in 54 days!
		if (hysteresis.expired())
			hysteresis.stop();
		return;
	}

	// There is a pending state change, but we might be in hysteresis.
	if (!hysteresis.expired())
		return;

	// Now we have a pending state change and an expired timer: we can change the output state.
	setOutputState(targetState);
}

/// <summary>
/// Sets the output state immediately and starts the hysteresis timer.
/// Since by definition we can only change output state if no hysteresis timer
/// is running, then we must also start the hysteresis timer so that the state is latched.
/// </summary>
/// <param name="state">The new output state.</param>
void HysteresisOutput::setOutputState(bool state)
{
	currentState = state;
	invokeUserStateChange(state); // Calls the user-supplied state change function
	hysteresis.setDuration(getHysteresisTime());
}

Duration HysteresisOutput::getHysteresisTime() { return currentState ? minimumTimeOn : minimumTimeOff; }

/// <summary>
/// Sets the input state. This may or may not result in the output state changing.
/// </summary>
/// <param name="requestedState">The state being requested.</param>
void HysteresisOutput::setInputState(bool requestedState)
{
	const bool returningToCurrentState = targetState != currentState && requestedState == currentState;
	targetState = requestedState;

	// No hysteresis delay, immediate change in output state required.
	if (!hysteresis.enabled())
	{
		/*
		Setting the output state is expected to be idempotent, but we only call it
		when there is a state change because it makes for cleaner debugging output.
		*/
		if (currentState != requestedState)
			setOutputState(requestedState);
		return;
	}

	// A hysteresis timer is running and state change must be deferred
	// However if we've changed away and back to the current state, the timer gets restarted.
	if (returningToCurrentState)
	{
		hysteresis.setDuration(getHysteresisTime());
	}
}
