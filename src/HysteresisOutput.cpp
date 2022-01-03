/*
 Name:		HysteresisOutput.cpp
 Author:	Tim Long (TimTek)
 Editor:	http://www.visualmicro.com
*/

#include "HysteresisOutput.h"
#include <functional>

void HysteresisOutput::turnOn()
    {
    targetState = true;
    if (timeSinceOff.enabled()) // There is a hysteresis delay in effect
        return;
    setOutputState();
    }

void HysteresisOutput::turnOff()
    {
    targetState = false;
    if (timeSinceOn.enabled()) // There is a hysteresis delay in effect
        return;
    setOutputState();
    }

void HysteresisOutput::loop()
    {
    if (timeSinceOff.expired() || timeSinceOn.expired())
        setOutputState();
    }

void HysteresisOutput::setLatchTimes(Duration onTime, Duration offTime)
    {
    minimumTimeOn = onTime;
    minimumTimeOff = offTime;
    }

/// <summary>
/// Sets the output state immediately and starts the hysteresis timer.
/// Since by definition we can only change output state if no hysteresis timer
/// is running, then we must also start the hysteresis timer so that the state is latched.
/// </summary>
/// <param name="state">The new output state.</param>
void HysteresisOutput::setOutputState()
    {
    if (targetState != currentState)
        {
        // Call the user state-change function, but only for actual state changes.
        invokeUserStateChange(targetState);
        currentState = targetState;
        }

    // Trigger the appropriate latch timer.
    if (currentState)
        {
        timeSinceOn.setDuration(minimumTimeOn);
        timeSinceOff.stop();
        }
    else
        {
        timeSinceOff.setDuration(minimumTimeOff);
        timeSinceOn.stop();
        }
    }

bool HysteresisOutput::getOutputState()
    {
    return currentState;
    }

Duration HysteresisOutput::getHysteresisTime()
    {
    return currentState ? minimumTimeOn : minimumTimeOff;
    }

/// <summary>
/// Sets the input state. This may or may not result in the output state changing.
/// </summary>
/// <param name="requestedState">The state being requested.</param>
void HysteresisOutput::setInputState(bool requestedState)
    {
    if (requestedState == true)
        turnOn();
    else
        turnOff();
    }
