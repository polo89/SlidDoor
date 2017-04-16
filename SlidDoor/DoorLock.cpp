// 
// 
// 

#include "DoorLock.h"
#include <Button.h>

DoorLock::DoorLock(int pin_LockState, int pin_LockTrigger, int pin_LockDir) : LockState(pin_LockState, PULLDOWN)
{
	_pinLockDir = pin_LockDir;
	_pinLockTrigger = pin_LockTrigger;
	pinMode(pin_LockTrigger, OUTPUT);
	pinMode(pin_LockDir, OUTPUT);
}

void DoorLock::Lock() {
	if (GetState() == UNLOCKED)
	{
		digitalWrite(_pinLockDir, LOW);
		delay(20);
		digitalWrite(_pinLockTrigger, HIGH);
		delay(20);
		digitalWrite(_pinLockTrigger, LOW);
	}
	else
	{
		digitalWrite(_pinLockTrigger, LOW);
		digitalWrite(_pinLockDir, LOW);
	}
}

void DoorLock::Unlock() {
	if (GetState() == LOCKED)
	{
		digitalWrite(_pinLockDir, HIGH);
		delay(20);
		digitalWrite(_pinLockTrigger, HIGH);
		delay(20);
		digitalWrite(_pinLockTrigger, LOW);
		delay(20);
		digitalWrite(_pinLockDir, LOW);
	}
	else
	{
		digitalWrite(_pinLockTrigger, LOW);
		digitalWrite(_pinLockDir, LOW);
	}
}

LockStates DoorLock::GetState() {
	if (LockState.isPressed()) return LOCKED;
	else return UNLOCKED;
}

