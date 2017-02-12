// 
// 
// 

#include "DoorLock.h"
#include <Button.h>

DoorLock::DoorLock(int pin_LockState, int pin_LockTrigger, int pin_LockDir) : LockState(pin_LockState, PULLUP)
{
	_pinLockDir = pin_LockDir;
	_pinLockTrigger = pin_LockTrigger;
	pinMode(pin_LockTrigger, OUTPUT);
	pinMode(pin_LockDir, OUTPUT);
}

void DoorLock::Lock() {
	if (GetState() != LOCKED)
	{
		digitalWrite(_pinLockDir, LOW);
		delay(20);
		while (GetState() != LOCKED) {
			digitalWrite(_pinLockTrigger, HIGH);		}
	}
	digitalWrite(_pinLockTrigger, LOW);
	digitalWrite(_pinLockDir, LOW);
}

void DoorLock::Unlock() {
	if (GetState() != UNLOCKED)
	{
		digitalWrite(_pinLockDir, HIGH);
		delay(20);
		while (GetState() != UNLOCKED) {
			digitalWrite(_pinLockTrigger, HIGH);
		}
	}
	digitalWrite(_pinLockTrigger, LOW);
	digitalWrite(_pinLockDir, LOW);
}

LockStates DoorLock::GetState() {
	if (LockState.isPressed()) return UNLOCKED;
	else return LOCKED;
}

