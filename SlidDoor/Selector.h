// Selector.h

#ifndef _SELECTOR_h
#define _SELECTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MODE_UNKNOWN B100
#define MODE_MANUAL B000
#define MODE_OPEN B001
#define MODE_SUMMER B011
#define	MODE_WINTER B010
#define MODE_ONEWAY B110
#define MODE_LOCK B111
#define MODE_LEARN B101

enum LEDSTATES {
	LEDSTATE_NORMAL, //leuchtet schwach
	LEDSTATE_NORMAL_LOCKED, //leuchtet hell
	LEDSTATE_LOCK_ERROR, //blinkt ungeleichm‰ﬂig
	LEDSTATE_MANUAL_LEARN, //blinkt gleichm‰ﬂig langsam
	LEDSTATE_MOTOR_MCB //blinkt gleich‰ﬂig schnell
};

class Selector
{
 private:
	 int _pin_Mode1;
	 int _pin_Mode2;
	 int _pin_Mode3;
	 int _pin_LED;
	 int _counterBlink;
	 bool _blink;
	 bool _lastBlink;
	 bool _modeChange;
	 bool _init;
	 byte _currentMode;
	 byte _lastMode;
	 LEDSTATES _currentState;
	 unsigned long _nextModeTime;
	 unsigned long _lastBlinkTime;
	 void blink(int intervall);

 public:
	 Selector(int pin_Mode1, int pin_Mode2, int pin_Mode3, int pin_LED);
	 byte GetMode();
	 bool ModeChange();
	 void SetLedState(LEDSTATES state);
};

#endif

