#include "CurrSens.h"

CurrSens::CurrSens(int pin, int sampleRate)
{
	_pin = pin;
	_sampleRate = sampleRate;
	pinMode(_pin, INPUT);
}

double CurrSens::Read() {
	double sample = 0;
	for (int i = 0; i < _sampleRate; i++)
	{
		sample += analogRead(_pin);
	}
	sample = sample / _sampleRate;
	double volt = (sample / 1024.0) * 5000; // Gets you mV
	double diffVolt = volt - _offset;
	double current = abs(diffVolt / 185.0);
	/*Serial.print(current);
	Serial.println("A");*/
	return current;
}

void CurrSens::Calibrate() {
	Serial.println("Starte Stromsensor Kalibrierung ...");
	double offset = 0;
	int offsetSampleRate = 1000;
	delay(100);
	for (int i = 0; i < offsetSampleRate; i++) {
		offset += analogRead(_pin);
	}
	offset = offset / offsetSampleRate;
	_offset = offset / 1024 * 5000;
	Serial.print("Stromsensor Offset: ");
	Serial.print(_offset);
	Serial.println("mV");
}