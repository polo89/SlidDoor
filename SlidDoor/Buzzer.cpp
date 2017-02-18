// 
// 
// 

#include "Buzzer.h"

Buzzer::Buzzer(int pin_Buzzer, unsigned long defaultIntervall)
{
	_pin = pin_Buzzer;
	_start = true;
	_defaultIntervall = defaultIntervall;
	pinMode(_pin, OUTPUT);
}

bool Buzzer::Beep() {
	if (_start)
	{
		_startBeepTime = millis();
		_start = false;
	}
	if (millis() - _startBeepTime > _defaultIntervall) {
		digitalWrite(_pin, HIGH);
		_finish = false;
	}
	else
	{
		digitalWrite(_pin, LOW);
		_finish = true;
	}
	return _finish;
}

bool Buzzer::Beep(int times) {

}

bool Buzzer::Beep(int times, int intervall) {

}
