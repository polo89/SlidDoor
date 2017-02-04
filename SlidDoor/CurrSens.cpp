#include "CurrSens.h"

CurrSens::CurrSens(int pin)
{
	_pin = pin;
	pinMode(_pin, INPUT);
}

double CurrSens::Read() {
	double sample = 0;
	for (int i = 0; i < 200; i++)
	{
		sample += analogRead(_pin);
	}
	sample = sample / 200;
	double volt = (sample / 1024.0) * 5000; // Gets you mV
	return ((volt - _offset) / _mVperA);
}