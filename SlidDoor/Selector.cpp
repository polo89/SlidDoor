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
	_lastBlinkTime = 0;
	_counterBlink = 0;
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
	switch (state)
	{
	case LEDSTATE_NORMAL:
		analogWrite(_pin_LED, 40);
		break;
	case LEDSTATE_NORMAL_LOCKED:
		analogWrite(_pin_LED, 255);
		break;
	case LEDSTATE_LOCK_ERROR:
		int t_intervall;
		t_intervall = 250;
		if (_blink != _lastBlink) {
			_lastBlink = _blink;
			_counterBlink++;
		}
		if (_counterBlink > 2) t_intervall = 1000;
		if (_counterBlink > 3) _counterBlink = 0;
		blink(t_intervall);
		break;
	case LEDSTATE_MANUAL_LEARN:
		blink(1000);
		break;
	case LEDSTATE_MOTOR_MCB:
		blink(150);
		break;
	default:
		break;
	}
}

void Selector::blink(int intervall) {
	if ((millis() - _lastBlinkTime) > intervall)
	{
		_lastBlinkTime = millis();
		if (_blink)
		{
			_blink = false;
			digitalWrite(_pin_LED, HIGH);
		}
		else {
			_blink = true;
			digitalWrite(_pin_LED, LOW);
		}
	}
}
