// 
// 
// 

#include "Selector.h"

Selector::Selector(int pin_Mode1, int pin_Mode2, int pin_Mode3, int pin_LED)
{
	_pin_Mode1 = pin_Mode1;
	_pin_Mode2 = pin_Mode2;
	_pin_Mode3 = pin_Mode3;
	_pin_LED = pin_LED;
	pinMode(_pin_Mode1, INPUT_PULLUP);
	pinMode(_pin_Mode2, INPUT_PULLUP);
	pinMode(_pin_Mode3, INPUT_PULLUP);
	pinMode(_pin_LED, OUTPUT);
	_modeChange = true;
}

byte Selector::GetMode() {
	bool mode1 = digitalRead(_pin_Mode1);
	bool mode2 = digitalRead(_pin_Mode2);
	bool mode3 = digitalRead(_pin_Mode3);
	byte _mode = mode1*pow(2, 2) + mode2*pow(2, 1) + mode3*pow(2, 0);
	if (_mode != _lastMode)
	{
		_nextModeTime = millis();
		_lastMode = _mode;
	}
	if (millis() - _nextModeTime > 1000 && _currentMode != _mode)
	{
		_currentMode = _mode;
		_modeChange = true;

	}
	return _currentMode;
}

bool Selector::ModeChange() {
	if (!_modeChange) return false;
	_modeChange = false;
	return true;
}

void Selector::SetLedState(LEDSTATES state) {

}
