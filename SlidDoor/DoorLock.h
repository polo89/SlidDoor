// Lock.h

#ifndef _DOORLOCK_h
#define _DOORLOCK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Button.h>

enum LockStates
{
	LOCKED,
	UNLOCKED
};

class DoorLock
{
 private:
	 Button LockState;
	 int _pinLockDir;
	 int _pinLockTrigger;

 public:
	DoorLock(int pin_LockState, int pin_LockTrigger, int pin_LockDir);
	void Lock();
	void Unlock();
	LockStates GetState();
};

#endif

