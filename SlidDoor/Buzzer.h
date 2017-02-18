// Buzzer.h

#ifndef _BUZZER_h
#define _BUZZER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Buzzer
{
 private:
	 int _pin;
	 unsigned long _startBeepTime;
	 unsigned long _lastBeepTime;
	 unsigned long _defaultIntervall;
	 bool _start;
	 bool _finish;

 public:
	 Buzzer(int pin_Buzzer, unsigned long defaultIntervall);
	 bool Beep();
	 bool Beep(int times);
	 bool Beep(int times, int intervall);
};

#endif

